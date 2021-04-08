#include <QtQml/qqml.h>
#include <QtQml/qqmlextensionplugin.h>

class TileViewPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void initializeEngine(QQmlEngine *, const char *) override {};
    void registerTypes(const char *) override {};
};

#include "tileviewplugin.moc"
