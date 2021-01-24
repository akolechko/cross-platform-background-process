#include "pch.h"

#include "service.h"

Service* Service::m_service = nullptr;

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wtsapi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


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
#elif linux
Service::Service()
  : m_isRunning(true)
{
    signal(SIGTERM, Service::SignalHandler);
}

bool Service::IsRunning() 
{
    return m_isRunning;
}

void Service::SignalHandler(int signal) 
{
    switch (signal) 
    {
        case SIGTERM:
            m_service->m_isRunning = false;
            break;
    }
}

bool Service::Run()
{     
    int listenPort = 1234;
 
    // Create a socket
    int s0 = socket(AF_INET, SOCK_STREAM, 0);
 
    // Fill in the address structure containing self address
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(listenPort);        // Port to listen
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    // Bind a socket to the address
    int res = bind(s0, (struct sockaddr*) &myaddr, sizeof(myaddr));
 
    // Set the "LINGER" timeout to zero, to close the listen socket
    // immediately at program termination.
    struct linger linger_opt = { 1, 0 }; // Linger active, timeout 0
    setsockopt(s0, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
 
    // Now, listen for a connection
    res = listen(s0, 1);    // "1" is the maximal length of the queue
 
    // Accept a connection (the "accept" command waits for a connection with
    // no timeout limit...)
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len;
    int s1 = accept(s0, (struct sockaddr*) &peeraddr, &peeraddr_len);
 
    // A connection is accepted. The new socket "s1" is created
    // for data input/output. The peeraddr structure is filled in with
    // the address of connected entity, print it.
 
    res = close(s0);    // Close the listen socket
 
    write(s1, "Hello!\r\n", 8);
 
    char buffer[1024];
    res = read(s1, buffer, 1023);
    buffer[res] = 0;
 
    close(s1); 
    return true;
}

#endif
