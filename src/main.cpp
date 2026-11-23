#include "application/Application.h"

int main(int argc, char** argv)
{
    if (const auto forwarded = CuteLight::Application::forwardCommand(argc, argv))
        return *forwarded;

    CuteLight::Application application(argc, argv);
    return application.run();
}
