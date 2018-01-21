#include "PlayerQuickItem.h"

#include <stdexcept>

#include <QCoreApplication>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QRunnable>
#include <QScreen>

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <QOpenGLFunctions>
#include <qpa/qplatformnativeinterface.h>

#include "QsLog.h"
#include "utils/Utils.h"
#include "Globals.h"

#if defined(Q_OS_WIN32)
#include <windows.h>
#include <dwmapi.h>
#include <avrt.h>
#endif

#if defined(USE_X11EXTRAS)
#include <QX11Info>
static void* MPGetNativeDisplay(const char* name)
{
  if (strcmp(name, "x11") == 0)
    return QX11Info::display();
}
#elif defined(USE_DRM)
static void* MPGetNativeDisplay(const char* name)
{
  static struct mpv_opengl_cb_window_pos pos;
  static struct mpv_opengl_cb_drm_params params;

  if (strcmp(name, "opengl-cb-window-pos") == 0)
  {
    QWindow *window = (QWindow*)Globals::MainWindow();
    if (window)
    {
        pos.x = window->geometry().left();
        pos.y = window->geometry().top();
        pos.width = window->geometry().width();
        pos.height = window->geometry().height();
        return &pos;
    }
  }
  if (strcmp(name, "opengl-cb-drm-params") == 0)
  {
    QGuiApplication *app = static_cast<QGuiApplication *>(QGuiApplication::instance());
    if (app)
    {
        void *p;
        p = app->platformNativeInterface()->nativeResourceForIntegration("dri_fd");
        if (p)
            params.fd = (int)(qintptr)p;

        QWindow *window = (QWindow*)Globals::MainWindow();
        p = app->platformNativeInterface()->nativeResourceForScreen("dri_crtcid", window->screen());
        if (p)
            params.crtc_id = (int)(qintptr)p;

        p = app->platformNativeInterface()->nativeResourceForIntegration("dri_atomic_request");
        if (p)
            params.atomic_request = (_drmModeAtomicReq *)(qintptr)p;

        return &params;
    }


  }
  return nullptr;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
static void* get_proc_address(void* ctx, const char* name)
{
  Q_UNUSED(ctx);

  QOpenGLContext* glctx = QOpenGLContext::currentContext();
  if (!glctx)
    return nullptr;

  void *res = (void *)glctx->getProcAddress(QByteArray(name));
  if (strcmp(name, "glMPGetNativeDisplay") == 0)
  {
#if defined(USE_X11EXTRAS) || defined(TARGET_ROCKCHIP)
    return (void *)&MPGetNativeDisplay;
#else
    return nullptr;
#endif
  }
#ifdef Q_OS_WIN32
  // wglGetProcAddress(), which is used by Qt, does not always resolve all
  // builtin functions with all drivers (only extensions). Qt compensates this
  // for a degree, but does this only for functions Qt happens to need. So
  // we need our own falback as well.
  if (!res)
  {
    HMODULE handle = (HMODULE)QOpenGLContext::openGLModuleHandle();
    if (handle)
      res = (void *)GetProcAddress(handle, name);
  }
#endif
  return res;
}

namespace {

/////////////////////////////////////////////////////////////////////////////////////////
class RequestRepaintJob : public QRunnable
{
public:
  explicit RequestRepaintJob(QQuickWindow *window) : m_window(window) { }

  void run() override
  {
    // QSGThreadedRenderLoop::update has a special code path that will render
    // without syncing the render and GUI threads unless asked elsewhere to support
    // QQuickAnimator animations. This is currently triggered by the fact that
    // QQuickWindow::update() is called from the render thread.
    // This allows continuing rendering video while the GUI thread is busy.
    //
    m_window->update();
  }

private:
  QQuickWindow *m_window;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
PlayerRenderer::PlayerRenderer(mpv::qt::Handle mpv, QQuickWindow* window)
: m_mpv(mpv), m_mpvGL(nullptr), m_window(window), m_size(), m_hAvrtHandle(nullptr), m_videoRectangle(-1, -1, -1, -1), m_fbo(0)
{
  m_mpvGL = (mpv_opengl_cb_context *)mpv_get_sub_api(m_mpv, MPV_SUB_API_OPENGL_CB);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerRenderer::init()
{
#ifdef Q_OS_WIN32
  // Request Multimedia Class Schedule Service.
  DwmEnableMMCSS(TRUE);
#endif

  mpv_opengl_cb_set_update_callback(m_mpvGL, on_update, (void *)this);

  // Signals presence of MPGetNativeDisplay().
  const char *extensions = "GL_MP_MPGetNativeDisplay";
  return mpv_opengl_cb_init_gl(m_mpvGL, extensions, get_proc_address, nullptr) >= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PlayerRenderer::~PlayerRenderer()
{
  // Keep in mind that the m_mpv handle must be held until this is done.
  if (m_mpvGL)
    mpv_opengl_cb_uninit_gl(m_mpvGL);
  delete m_fbo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerRenderer::render()
{
  QOpenGLContext *context = QOpenGLContext::currentContext();

  GLint fbo = 0;
  context->functions()->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
  bool flip = true;
#if HAVE_OPTIMALORIENTATION
  flip = !(context->format().orientationFlags() & QSurfaceFormat::MirrorVertically);
#endif
  bool screenFlip = flip;
  QSize fboSize = m_size;
  QOpenGLFramebufferObject *blitFbo = 0;

  m_window->resetOpenGLState();

  QRect fullWindow(0, 0, m_size.width(), m_size.height());
  if (m_videoRectangle.width() > 0 && m_videoRectangle.height() > 0 && m_videoRectangle != fullWindow && QOpenGLFramebufferObject::hasOpenGLFramebufferBlit() && QOpenGLFramebufferObject::hasOpenGLFramebufferObjects())
  {
    if (!m_fbo || !m_fbo->isValid() || m_fbo->size() != m_videoRectangle.size())
    {
      delete m_fbo;
      m_fbo = new QOpenGLFramebufferObject(m_videoRectangle.size());
    }
    if (m_fbo && m_fbo->isValid())
    {
      blitFbo = m_fbo;
      fboSize = m_fbo->size();
      fbo = m_fbo->handle();
      flip = false;

      // Need to clear the background manually, since nothing else knows it has to be done.
      context->functions()->glClearColor(0, 0, 0, 0);
      context->functions()->glClear(GL_COLOR_BUFFER_BIT);
    }
  }

  // The negative height signals to mpv that the video should be flipped
  // (according to the flipped OpenGL coordinate system).
  mpv_opengl_cb_draw(m_mpvGL, fbo, fboSize.width(), (flip ? -1 : 1) * fboSize.height());

  m_window->resetOpenGLState();

  if (blitFbo)
  {
    QRect dstRect = m_videoRectangle;
    if (screenFlip)
      dstRect = QRect(dstRect.x(), m_size.height() - dstRect.y(), dstRect.width(), dstRect.top() - dstRect.bottom());

    QOpenGLFramebufferObject::blitFramebuffer(0, dstRect, blitFbo, QRect(QPoint(0, 0), blitFbo->size()));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerRenderer::swap()
{
  mpv_opengl_cb_report_flip(m_mpvGL, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerRenderer::onVideoPlaybackActive(bool active)
{
#ifdef Q_OS_WIN32
  if (active && !m_hAvrtHandle)
  {
    DWORD handle = 0;
    m_hAvrtHandle = AvSetMmThreadCharacteristicsW(L"Low Latency", &handle);
  }
  else if (!active && m_hAvrtHandle)
  {
    AvRevertMmThreadCharacteristics(m_hAvrtHandle);
    m_hAvrtHandle = 0;
  }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerRenderer::on_update(void *ctx)
{
  PlayerRenderer *self = (PlayerRenderer *)ctx;
  // QQuickWindow::scheduleRenderJob is expected to be called from the GUI thread but
  // is thread-safe when using the QSGThreadedRenderLoop. We can detect a non-threaded render
  // loop by checking if QQuickWindow::beforeSynchronizing was called from the GUI thread
  // (which affects the QObject::thread() of the PlayerRenderer).
  //
  if (self->thread() == self->m_window->thread())
    QMetaObject::invokeMethod(self->m_window, "update", Qt::QueuedConnection);
  else
    self->m_window->scheduleRenderJob(new RequestRepaintJob(self->m_window), QQuickWindow::NoStage);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PlayerQuickItem::PlayerQuickItem(QQuickItem* parent)
: QQuickItem(parent), m_mpvGL(nullptr), m_renderer(nullptr)
{
  connect(this, &QQuickItem::windowChanged, this, &PlayerQuickItem::onWindowChanged, Qt::DirectConnection);
  connect(this, &PlayerQuickItem::onFatalError, this, &PlayerQuickItem::onHandleFatalError, Qt::QueuedConnection);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PlayerQuickItem::~PlayerQuickItem()
{
  if (m_mpvGL)
    mpv_opengl_cb_set_update_callback(m_mpvGL, nullptr, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerQuickItem::onWindowChanged(QQuickWindow* win)
{
  if (win)
  {
    connect(win, &QQuickWindow::beforeSynchronizing, this, &PlayerQuickItem::onSynchronize, Qt::DirectConnection);
    connect(win, &QQuickWindow::sceneGraphInvalidated, this, &PlayerQuickItem::onInvalidate, Qt::DirectConnection);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerQuickItem::onHandleFatalError(QString message)
{
  throw FatalException(message);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerQuickItem::onSynchronize()
{
  if (!m_renderer && m_mpv)
  {
    m_renderer = new PlayerRenderer(m_mpv, window());
    if (!m_renderer->init())
    {
      delete m_renderer;
      m_renderer = nullptr;
      emit onFatalError(tr("Could not initialize OpenGL."));
      return;
    }
    connect(window(), &QQuickWindow::beforeRendering, m_renderer, &PlayerRenderer::render, Qt::DirectConnection);
    connect(window(), &QQuickWindow::frameSwapped, m_renderer, &PlayerRenderer::swap, Qt::DirectConnection);
    connect(&PlayerComponent::Get(), &PlayerComponent::videoPlaybackActive, m_renderer, &PlayerRenderer::onVideoPlaybackActive, Qt::QueuedConnection);
    connect(&PlayerComponent::Get(), &PlayerComponent::onVideoRecangleChanged, window(), &QQuickWindow::update, Qt::QueuedConnection);
    window()->setPersistentOpenGLContext(true);
    window()->setPersistentSceneGraph(true);
    window()->setClearBeforeRendering(false);
    m_debugInfo = "";
    QOpenGLContext* glctx = QOpenGLContext::currentContext();
    if (glctx && glctx->isValid())
    {
      m_debugInfo += "\nOpenGL:\n";
      int syms[4] = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_SHADING_LANGUAGE_VERSION};
      for (auto sym : syms)
      {
        auto s = (char *)glctx->functions()->glGetString(sym);
        if (s)
          m_debugInfo += QString("  ") + QString::fromUtf8(s) + "\n";
      }
      m_debugInfo += "\n";
    }
  }
  if (m_renderer)
  {
    m_renderer->m_size = window()->size() * window()->devicePixelRatio();
    m_renderer->m_videoRectangle = PlayerComponent::Get().videoRectangle();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerQuickItem::onInvalidate()
{
  if (m_renderer)
    delete m_renderer;
  m_renderer = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerQuickItem::initMpv(PlayerComponent* player)
{
  m_mpv = player->getMpvHandle();

  m_mpvGL = (mpv_opengl_cb_context *)mpv_get_sub_api(m_mpv, MPV_SUB_API_OPENGL_CB);
  if (!m_mpvGL)
    throw FatalException(tr("OpenGL not enabled in libmpv."));

  connect(player, &PlayerComponent::windowVisible, this, &QQuickItem::setVisible);
  window()->update();
}
