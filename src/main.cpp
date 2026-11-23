#include "base/i18n.h"

#include "application/Application.h"

int main(int argc, char** argv)
{
    if (const auto forwarded = CutefishSearch::Application::forwardCommand(argc, argv))
        return *forwarded;

    CutefishSearch::Application application(argc, argv);
    application.setApplicationId("org.cutefish.search");
    cutefish::i18n::setDomain("cutefish-search");
    return application.run();
}
