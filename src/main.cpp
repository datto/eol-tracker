#include "frontend.h"

#include <QCoreApplication>
#include <Cutelyst/WSGI/wsgi.h>
#include <memory>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    auto myapp = std::make_unique<frontend>();
    auto wsgi = std::make_unique<CWSGI::WSGI>();
    wsgi->setHttpSocket({
        {QStringLiteral(":3000")},
    });
    wsgi->setUpgradeH2c(true);
    wsgi->setBufferSize(16393);
    wsgi->setHttp2Socket({
        {QStringLiteral(":3001")},
    });
    wsgi->setFastcgiSocket({
        {QStringLiteral(":3002")},
    });
    wsgi->setMaster(true);
    wsgi->setAutoReload(true);
    wsgi->setLazy(true);
    wsgi->exec(myapp.get());

    return 0;
}