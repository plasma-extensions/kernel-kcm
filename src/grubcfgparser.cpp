#include "grubcfgparser.h"

#include <QMap>
#include <QList>
#include <QChar>
#include <QFile>
#include <QTextStream>

#include <QDebug>

using namespace Grub;

GrubCFGParser::GrubCFGParser()
{
    _grubCFGFile = "/boot/grub/grub.cfg";
}

SubMenu GrubCFGParser::getMenuEntries()
{
    QFile grub_cfg(_grubCFGFile);

    SubMenu entries;
    if (grub_cfg.open(QFile::ReadOnly)) {
        QTextStream in(&grub_cfg);
        QString word;
        while ( !in.atEnd()) {
            word = readWord(in);
            if (word.compare("menuentry") == 0) {
                MenuEntry entry = readMenuEntry(in);

                entries << entry;
            }

            if (word.compare("submenu") == 0) {
                SubMenu subMenu = readSubMenu(in);
                entries << subMenu;
            }
        }
    } else {
        qDebug() << "ERROR: unable to open " << grub_cfg.fileName();
    }

    return entries;
}

SubMenu GrubCFGParser::getGnuLinuxEntries()
{
    SubMenu entries = getMenuEntries();
    SubMenu  gnuLinuxEntries ;

    for (MenuEntry entry: entries)
        if (entry.contains("isGnuLinux"))
            gnuLinuxEntries << entry;

    return gnuLinuxEntries;
}

QString GrubCFGParser::readWord(QTextStream &in)
{
    static QString buffer;
    QString word = "";
    QChar token;

    const static QString metaCharacters = "{}|&$;<> \t\n";
    const static QString scapeCharacters = "\\\'\"";

    while (!in.atEnd() && word.isEmpty()) { // Exclude empty words
        QChar scape = QChar::Null;

        do {
            in >> token;
            // qDebug() << "TOKEN: " << token;

            // Whipe scape flag
            if (scape == '\\' || scape == token) {
                scape = QChar::Null;
                continue;
            }

            if (scapeCharacters.contains(token)) {
                scape = token;
                continue;
            }

            buffer.append(token);
        } while ((!in.atEnd() && (!metaCharacters.contains(token) || !scape.isNull() )));


        word = buffer.left(buffer.length()-1);
        buffer = buffer.right(1);

        word = word.trimmed();
    }

    // qDebug() << "BUFFER" << buffer;
    // qDebug() << "WORD: " << word;
    return word;
}

MenuEntry GrubCFGParser::readMenuEntry(QTextStream &in)
{
    MenuEntry menuEntry;
    // READ title
    menuEntry["title"] = readWord(in);

    // READ PARAMS
    QString word = readWord(in);
    while (!in.atEnd() && !word.contains('{')) {
        if (word.compare("--class") == 0) {
            if (readWord(in).compare("gnu-linux") == 0)
                menuEntry["isGnuLinux"] = "true";
        }

        if (word.compare("$menuentry_id_option") == 0) {
            menuEntry["id"] = readWord(in);
        }

        word = readWord(in);
    }

    // READ COMMANDS
    while (!in.atEnd() && !word.contains('}')) {
        if (word.compare("linux") == 0 && menuEntry.contains("isGnuLinux")) {
            menuEntry["kernel"] = readWord(in);
        }
        word = readWord(in);
    }
    return menuEntry;
}

SubMenu GrubCFGParser::readSubMenu(QTextStream &in)
{
    SubMenu entries;
    // READ title
    readWord(in);

    // READ PARAMS
    QString id;
    QString word = readWord(in);
    while (!in.atEnd() && !word.contains('{')) {
        if (word.compare("$menuentry_id_option") == 0) {
            id = readWord(in);
        }
        word = readWord(in);
    }

    // READ Entries
    while (!in.atEnd() && !word.contains('}')) {
        if (word.compare("menuentry") == 0 ) {
            MenuEntry entry = readMenuEntry(in);

            entry["id"] =  id + ">"+entry["id"];
            entries << entry;
        }

        word = readWord(in);
    }

    return entries;

}

