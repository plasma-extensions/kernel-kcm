#ifndef GRUBCFGPARSER_H
#define GRUBCFGPARSER_H

#include <QMap>
#include <QList>
#include <QString>
#include <QTextStream>

namespace Grub {
    typedef QMap<QString, QString> MenuEntry;
    typedef QList<MenuEntry> SubMenu;

    /**
     * @brief The GrubCFGParser class
     *
     * Not pretending to be a full parsers.
     */

    class GrubCFGParser
    {
    public:
        GrubCFGParser();

        SubMenu getMenuEntries();
        SubMenu getGnuLinuxEntries();

    private:
        QString readWord(QTextStream &in);
        MenuEntry readMenuEntry(QTextStream &in);
        SubMenu readSubMenu(QTextStream &in);

        QString _grubCFGFile;
    };

}
#endif // GRUBCFGPARSER_H
