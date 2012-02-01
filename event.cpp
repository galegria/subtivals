#include <QtCore/QStringList>

#include "event.h"
#include "script.h"

Event::Event(const QString &p_line, const Script *p_script, int p_index, QObject *p_parent) :
    QObject(p_parent)
{
    QList<QString> subparts = p_line.split(',');
    m_msseStart = QTime().msecsTo(QTime::fromString(subparts[1], "h:mm:ss.z"));
    m_msseEnd = QTime().msecsTo(QTime::fromString(subparts[2], "h:mm:ss.z"));
    m_style = p_script->style(subparts[3].trimmed());

    // Check if style is overridden in event
    m_marginL = subparts[5].toInt();
    m_marginR = subparts[6].toInt();
    m_marginV = subparts[7].toInt();

    // Check if no timecode is specified
    if (m_msseStart == 0 && m_msseStart == m_msseEnd) {
        m_msseStart = 1000 * p_index;
        m_msseEnd = m_msseStart + 500;
    }

    int p = p_line.indexOf(",");
    for (int i = 0; i < 8; i++)
    {
        p = p_line.indexOf(",", p+1);
    }
    m_prettyText = p_line.mid(p+1);
    m_text = m_prettyText;

    m_prettyText = m_prettyText.replace("\\N", QString::fromUtf8(" ↲ "));
    m_prettyText = m_prettyText.replace("\\n", QString::fromUtf8(" ↲ "));
    m_prettyText = m_prettyText.replace("{\\i1}", "**");
    m_prettyText = m_prettyText.replace("{\\i0}", "**");
    // Drop others hints that cannot be translated in HTML
    {
        int idxAccOpenDrop = m_prettyText.indexOf("{\\");
        while (idxAccOpenDrop != -1)
        {
            int idxAccCloseDrop = m_prettyText.indexOf("}", idxAccOpenDrop);
            if (idxAccCloseDrop != -1)
            {
                m_prettyText = m_prettyText.left(idxAccOpenDrop) + m_prettyText.mid(idxAccCloseDrop+1);
            }
            idxAccOpenDrop = m_prettyText.indexOf("{\\");
        }
    }

    // Transform the hints in the text into HTML:
    // New ligne HTML-ification
    m_text = m_text.replace("\\N", "<br/>");
    m_text = m_text.replace("\\n", "<br/>");
    // Italic HTML-ification
    m_text = m_text.replace("{\\i1}", "<i>");
    m_text = m_text.replace("{\\i0}", "</i>");
    // Color HTML-ification
    {
        int idxAccOpenColor = m_text.indexOf("{\\1c&H");
        while (idxAccOpenColor != -1)
        {

            int idxAccCloseColor = m_text.indexOf("}", idxAccOpenColor);
            if (idxAccCloseColor != -1)
            {
                QString htmlColor = "<font color=\"#" + m_text.mid(idxAccOpenColor + 6 + 4, 2) + m_text.mid(idxAccOpenColor + 6 + 2, 2) + m_text.mid(idxAccOpenColor + 6, 2) + "\">";
                m_text = m_text.left(idxAccOpenColor) + htmlColor + m_text.mid(idxAccCloseColor+1) + "</font>";
            }
            idxAccOpenColor = m_text.indexOf("{\\1c&H");
        }
    }
    // Drop others hints that cannot be translated in HTML
    {
        int idxAccOpenDrop = m_text.indexOf("{\\");
        while (idxAccOpenDrop != -1)
        {
            int idxAccCloseDrop = m_text.indexOf("}", idxAccOpenDrop);
            if (idxAccCloseDrop != -1)
            {
                m_text = m_text.left(idxAccOpenDrop) + m_text.mid(idxAccCloseDrop+1);
            }
            idxAccOpenDrop = m_text.indexOf("{\\");
        }
    }
    m_text = m_text.trimmed();
    while(m_text.startsWith("\n"))
    {
        m_text = m_text.mid(1);
    }
}

qint64 Event::msseStart() const
{
    return m_msseStart;
}

qint64 Event::msseEnd() const
{
    return m_msseEnd;
}

const Style *Event::style() const
{
    return m_style;
}

const QString &Event::text() const
{
    return m_text;
}

const QString &Event::prettyText() const
{
    return m_prettyText;
}

bool Event::match(qint64 p_msecs) const
{
    return m_msseStart <= p_msecs && m_msseEnd >= p_msecs;
}

int Event::marginL() const
{
    return m_marginL;
}

int Event::marginR() const
{
    return m_marginR;
}

int Event::marginV() const
{
    return m_marginV;
}
