#include "SaveTokenAgent.h"

#include "Utils\Patterns\PublisherSubscriber\Broker.h"
#include "../../Network/Events.h"
#include "..\..\Network\Model\Credentials.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <sstream>

namespace desktop { namespace core { namespace agent {

	SaveTokenAgent::SaveTokenAgent(std::unique_ptr<service::FileIOService> fileIOService, std::unique_ptr<service::ApplicationDataService> applicationService)
	: m_fileIOService(std::move(fileIOService))
	, m_applicationService(std::move(applicationService))
	{
		m_subscriber.subscribe([this](const desktop::core::utils::patterns::Event& rawEvt)
		{
			const auto& evt = static_cast<const core::events::CredentialsEvent&>(rawEvt);

			auto credentials = std::make_unique<model::Credentials>(evt.m_credentials);

			auto viewerFolder = m_applicationService->getViewerFolder();
			
			try
			{
				boost::property_tree::ptree tree;

				{
					boost::property_tree::ptree token;
					tree.add_child("token", boost::property_tree::ptree{ credentials->m_token });
				}

				{
					boost::property_tree::ptree host;
					tree.add_child("host", boost::property_tree::ptree{ credentials->m_host });
				}

				{
					boost::property_tree::ptree port;
					tree.add_child("port", boost::property_tree::ptree{ credentials->m_port });
				}

				boost::property_tree::json_parser::write_json(viewerFolder + "\\token.json", tree);
			}
			catch (...)
			{
				
			}
			
		}, events::CREDENTIALS_EVENT);
	}

	SaveTokenAgent::~SaveTokenAgent()
	{
		
	}
}}}