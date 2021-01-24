#pragma once

#ifdef _WIN64
struct ServiceParameters
{
    const CString& name         = "CrossPlatformBackgroundProcess";
    const CString& display_name = "Cross-platform background process";
    DWORD start_type            = SERVICE_DEMAND_START;
    DWORD err_ctrl_type         = SERVICE_ERROR_NORMAL;
    DWORD accepted_cmds         = SERVICE_ACCEPT_STOP;
};
#endif

class Service
{
public:
#ifdef _WIN64
    explicit Service(const ServiceParameters& parameters);
#elif linux
    explicit Service();
#endif

    Service(const Service& other) = delete;
    Service& operator=(const Service& other) = delete;

    Service(Service&& other) = delete;
    Service& operator=(Service&& other) = delete;

#ifdef _WIN64
    const CString& GetName() const;
    const CString& GetDisplayName() const;
    const DWORD GetStartType() const;
    const DWORD GetErrorControlType() const;
#endif
  
    bool Run();
    bool IsRunning();

private:
    static Service* m_service;
    bool m_isRunning;
    static void SignalHandler(int signal);
#ifdef _WIN64
    static DWORD WINAPI ServiceCtrlHandler(
        DWORD control_code, 
        DWORD event_type,
        void* event_data, 
        void* context);

    static void WINAPI SvcMain(DWORD argc, CHAR** argv);

    void SetStatus(
        DWORD state,
        DWORD exit_code = NO_ERROR,
        DWORD wait_hint = 0);

    void Start(DWORD argc, CHAR** argv);
    void Stop();
    void OnStart(DWORD, CHAR**);
    void OnStop();
    
private:
    CString m_name;
    CString m_display_name;
    DWORD m_start_type;
    DWORD m_error_control_type;
    SERVICE_STATUS m_status;
    SERVICE_STATUS_HANDLE m_status_handle;
    std::thread m_main_thread;
#endif
};
