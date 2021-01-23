#include "pch.h"

#include "service.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wtsapi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

Service* Service::m_service = nullptr;

Service::Service(const ServiceParameters& parameters)
  : m_name(parameters.name),
    m_display_name(parameters.display_name),
    m_start_type(parameters.start_type),
    m_error_control_type(parameters.err_ctrl_type),
    m_status_handle(nullptr),
    m_status{
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_START_PENDING,
        parameters.accepted_cmds,
        NO_ERROR,
        0,
        0,
        0}
{ }

void Service::SetStatus(DWORD state, DWORD error_code, DWORD wait_hint)
{
    m_status.dwCurrentState = state;
    m_status.dwWin32ExitCode = error_code;
    m_status.dwWaitHint = wait_hint;

    ::SetServiceStatus(m_status_handle, &m_status);
}

DWORD WINAPI Service::ServiceCtrlHandler(
    DWORD control_code, DWORD event_type,
    void* event_data, void* context) 
{
    if (control_code == SERVICE_CONTROL_STOP)
    {
        m_service->Stop();
    }

    return 0;
}

void WINAPI Service::SvcMain(DWORD argc, CHAR** argv) 
{
    assert(m_service);

    m_service->m_status_handle = ::RegisterServiceCtrlHandlerEx(
        m_service->GetName(),
        ServiceCtrlHandler,
        NULL);

    if (!m_service->m_status_handle)
    {
        return;
    }

    m_service->Start(argc, argv);
}

bool Service::Run()
{
    m_service = this;

    CHAR* name = const_cast<CString&>(m_name).GetBuffer();

    SERVICE_TABLE_ENTRY table_entry[] =
    {
        { name, SvcMain },
        { nullptr, nullptr }
    };

    return ::StartServiceCtrlDispatcher(table_entry) == TRUE;
}

const CString& Service::GetName() const 
{ 
    return m_name; 
}

const CString& Service::GetDisplayName() const 
{ 
    return m_display_name;
}

const DWORD Service::GetStartType() const
{ 
    return m_start_type;
}

const DWORD Service::GetErrorControlType() const
{
    return m_error_control_type;
}

void Service::OnStart(DWORD, CHAR**)
{
    
}

void Service::OnStop() 
{
    m_main_thread.join();
}

void Service::Start(DWORD argc, CHAR** argv)
{
    SetStatus(SERVICE_START_PENDING);
    OnStart(argc, argv);
    SetStatus(SERVICE_RUNNING);
}

void Service::Stop() 
{
    SetStatus(SERVICE_STOP_PENDING);
    OnStop();
    SetStatus(SERVICE_STOPPED);
}
