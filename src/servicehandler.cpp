#include "pch.h"

#include "servicehandler.h"
#include "utils.h"

#ifdef WIN32
class ServiceHandle
{
public:
    explicit ServiceHandle(SC_HANDLE handle)
      : m_handle(handle) {}

    ~ServiceHandle() 
    {
        if (m_handle != nullptr) 
        {
            if(::CloseServiceHandle(m_handle) == 0)
            {
                Utils::DisplayError("Failed to close the service handle");
            };
        }
    }

    SC_HANDLE GetHandle() const
    {
        return m_handle;
    }

private:
    SC_HANDLE m_handle = nullptr;
};

ServiceHandler::ServiceHandler(std::unique_ptr<Service> service)
  : m_service(std::move(service))
{ }

bool ServiceHandler::Install() const
{
    Utils::DisplayMessage("Installing service");

    bool success = true;

    do
    {
        CString escaped_path;
        LPSTR module_path = escaped_path.GetBufferSetLength(MAX_PATH);

        if (::GetModuleFileName(nullptr, module_path, MAX_PATH) == 0)
        {
            Utils::DisplayError("Failed to get module file name");
            success = false;
            break;
        }

        escaped_path.ReleaseBuffer();
        escaped_path.Remove('\"');

        escaped_path = '\"' + escaped_path + '\"';

        auto service_control_manager = std::make_unique<ServiceHandle>(
            ::OpenSCManager(
                nullptr, nullptr,
                SC_MANAGER_CONNECT | 
                SC_MANAGER_CREATE_SERVICE));

        if (service_control_manager->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open service control manager");
            success = false;
            break;
        }

        auto service = std::make_unique<ServiceHandle>(
            ::CreateService(
                service_control_manager->GetHandle(),
                m_service->GetName(),
                m_service->GetDisplayName(),
                SERVICE_QUERY_STATUS | SERVICE_START,
                SERVICE_WIN32_OWN_PROCESS,
                m_service->GetStartType(),
                m_service->GetErrorControlType(),
                escaped_path,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr));

        if (service->GetHandle() == nullptr) 
        {
            Utils::DisplayError("Failed to create service");
            success = false;
            break;
        }

        Start();
    } while (false);
    
    success ? Utils::DisplayMessage("Service installed")
            : Utils::DisplayMessage("Failed to install the service");

    return success;
}

bool ServiceHandler::Uninstall() const
{
    Utils::DisplayMessage("Uninstalling service");

    bool success = true;

    do
    {
        auto service_control_manager = std::make_unique<ServiceHandle>(
            ::OpenSCManager(
                nullptr, nullptr,
                SC_MANAGER_CONNECT));

        if (service_control_manager->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open service control manager");
            success = false;
            break;
        }

        auto service = std::make_unique<ServiceHandle>(
            ::OpenService(
                service_control_manager->GetHandle(), 
                m_service->GetName(),
                SERVICE_QUERY_STATUS | 
                DELETE));

        if (service->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open service");
            success = false;
            break;
        }

        Stop();

        if (::DeleteService(service->GetHandle()) == 0)
        {
            Utils::DisplayError("Failed to delete the service");
            success = false;
            break;
        }    
    } while (false);
    
    success ? Utils::DisplayMessage("Service uninstalled")
            : Utils::DisplayMessage("Failed to uninstall the service");

    return success;
}

bool ServiceHandler::Start() const
{
    Utils::DisplayMessage("Starting the service");

    bool success = true;

    do
    {
        auto service_control_manager = std::make_unique<ServiceHandle>(
            ::OpenSCManager(
                nullptr, nullptr,
                SC_MANAGER_CONNECT));

        if (service_control_manager->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open the service control manager");
            success = false;
            break;
        }

        auto service = std::make_unique<ServiceHandle>(
            ::OpenService(
                service_control_manager->GetHandle(), 
                m_service->GetName(),
                SERVICE_QUERY_STATUS | 
                SERVICE_START));

        if (service->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open the service");
            success = false;
            break;
        }

        if (::StartService(service->GetHandle(), 0, NULL) == 0)
        {
            Utils::DisplayError("Failed to start the service");
            success = false;
            break;
        }
    } while (false);
    
    success ? Utils::DisplayMessage("Service started")
            : Utils::DisplayMessage("Failed to start the service");

    return success;
}

bool ServiceHandler::Stop() const
{
    Utils::DisplayMessage("Stopping the service");

    bool success = true;

    do
    {
        auto service_control_manager = std::make_unique<ServiceHandle>(
            ::OpenSCManager(
                nullptr, nullptr,
                SC_MANAGER_CONNECT));

        if (service_control_manager->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open the service control manager");
            success = false;
            break;
        }

        auto service = std::make_unique<ServiceHandle>(
            ::OpenService(
                service_control_manager->GetHandle(), 
                m_service->GetName(),
                SERVICE_QUERY_STATUS | 
                SERVICE_STOP));

        if (service->GetHandle() == nullptr)
        {
            Utils::DisplayError("Failed to open the service");
            success = false;
            break;
        }

        SERVICE_STATUS status = {};
        if (::ControlService(service->GetHandle(), SERVICE_CONTROL_STOP, &status) != 0)
        {    
            while (::QueryServiceStatus(service->GetHandle(), &status) != 0)
            {
                if (status.dwCurrentState != SERVICE_STOP_PENDING)
                {
                    break;
                }
            }

            if (status.dwCurrentState != SERVICE_STOPPED)
            {
                success = false;
                break;
            } 
        } 
        else
        {
            Utils::DisplayError("Didn't control service");
        } 
    } while (false);
    
    success ? Utils::DisplayMessage("Service stopped")
            : Utils::DisplayMessage("Failed to stop the service");

    return success;
}

bool ServiceHandler::Restart() const
{
    Utils::DisplayMessage("Starting the service");

    bool success = true;

    if (success = Stop())
    {
        success = Start();
    }
    
    success ? Utils::DisplayMessage("Service restarted")
            : Utils::DisplayMessage("Failed to restart the service");

    return success;
}

bool ServiceHandler::Run() const
{
    return m_service->Run();
}
#endif
