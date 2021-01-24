#pragma once

#include "service.h"

#ifdef WIN32
class ServiceHandler
{
public:
	explicit ServiceHandler(std::unique_ptr<Service> service);

    ServiceHandler(const ServiceHandler& other) = delete;
    ServiceHandler& operator=(const ServiceHandler& other) = delete;

    ServiceHandler(ServiceHandler&& other) = delete;
    ServiceHandler& operator=(ServiceHandler&& other) = delete;

	bool Install() const;
	bool Uninstall() const;

	bool Run() const;

private:
	bool Start() const;
	bool Stop() const;
	bool Restart() const;

private:
	std::unique_ptr<Service> m_service;
};
#endif
