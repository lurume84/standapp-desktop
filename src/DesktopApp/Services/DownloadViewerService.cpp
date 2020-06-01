
#include "DownloadViewerService.h"

#include "Events.h"
#include "DesktopCore\Upgrade\Events.h"
#include "DesktopCore\Utils\Patterns\PublisherSubscriber\Broker.h"

#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4481)
#include <include/cef_app.h>
#pragma warning(pop)

namespace desktop { namespace ui { namespace service {

	DownloadViewerService::DownloadViewerService(CefBrowser& browser, std::unique_ptr<core::service::EncodeStringService> encodeService,
											std::unique_ptr<core::service::ApplicationDataService> applicationService,
											std::unique_ptr<core::service::IniFileService> iniFileService)
	: m_browser(browser)
	, m_encodeService(std::move(encodeService))
	, m_applicationService(std::move(applicationService))
	, m_iniFileService(std::move(iniFileService))
	{
		m_subscriber.subscribe([this](const desktop::core::utils::patterns::Event& rawEvt)
		{
			auto evt = static_cast<const events::DownloadStatusEvent&>(rawEvt);

			if (evt.bIsComplete)
			{
				m_path = evt.m_path;

				m_cv.notify_one();
			}

		}, events::DOWNLOAD_STATUS_EVENT);

		m_subscriber.subscribe([this](const desktop::core::utils::patterns::Event& rawEvt)
		{
			auto evt = static_cast<const desktop::core::events::UpgradeViewerCompletedEvent&>(rawEvt);
			
			if (!evt.m_fresh)
			{
				std::stringstream ss;
				ss << "$(document).trigger('upgrade', '" << evt.m_version << "');";

				m_browser.GetMainFrame()->ExecuteJavaScript(ss.str(), "", 0);
			}
			else
			{ 
				auto fileServer = m_iniFileService->get<std::string>(m_applicationService->getMyDocuments() + "Bling.ini", "FileServer", "Endpoint", "http://127.0.0.1:9191/");
				m_browser.GetMainFrame()->LoadURL(fileServer);
			}
		}, desktop::core::events::UPGRADE_VIEWER_COMPLETED_EVENT);
	}

	DownloadViewerService::~DownloadViewerService() = default;

	std::string DownloadViewerService::download(const std::string& url, std::map<std::string, std::string> requestHeaders, const std::string &/*folder*/) const
	{
		std::string script = "window.location = '" + url + "';";

		m_browser.GetMainFrame()->ExecuteJavaScript(script, m_browser.GetMainFrame()->GetURL(), 0);

		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock);

		return m_path;
	}

	std::string DownloadViewerService::download(const std::string& host, const std::string& url, std::map<std::string, std::string> requestHeaders, const std::string &/*folder*/) const
	{
		auto pos = url.find(host) + host.size();

		std::string script = "window.location = 'https://" + host + url.substr(pos) + "';";

		m_browser.GetMainFrame()->ExecuteJavaScript(script, m_browser.GetMainFrame()->GetURL(), 0);

		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock);

		return m_path;
	}

	void DownloadViewerService::cancel()
	{
		m_cv.notify_all();
	}
}}}