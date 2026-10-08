#include "History.h"

#include <algorithm>

History::History(QObject *parent) : QObject(parent) {}

qint64 History::bytesOf(const ImagePtr &p)
{
    return p ? static_cast<qint64>(p->width) * p->height * p->channels : 0;
}

void History::reset(const QString &name, ImagePtr image)
{
    m_entries.clear();
    m_entries.push_back({name, std::move(image)});
    m_index = 0;
    emit changed();
    emit currentImageChanged();
}

void History::clear()
{
    m_entries.clear();
    m_index = -1;
    emit changed();
    emit currentImageChanged();
}

void History::push(const QString &name, ImagePtr image)
{
    if (m_index < 0) {
        reset(name, std::move(image));
        return;
    }
    m_entries.erase(m_entries.begin() + m_index + 1, m_entries.end());
    m_entries.push_back({name, std::move(image)});
    m_index = static_cast<int>(m_entries.size()) - 1;
    trim();
    emit changed();
    emit currentImageChanged();
}

void History::trim()
{
    auto total = [this]() {
        qint64 sum = 0;
        for (const Entry &e : m_entries)
            sum += bytesOf(e.image);
        return sum;
    };
    while (m_entries.size() > 2 &&
           (static_cast<int>(m_entries.size()) > m_maxEntries || total() > m_maxBytes)) {
        m_entries.erase(m_entries.begin());
        --m_index;
    }
}

void History::undo()
{
    if (canUndo())
        jumpTo(m_index - 1);
}

void History::redo()
{
    if (canRedo())
        jumpTo(m_index + 1);
}

void History::jumpTo(int index)
{
    if (index < 0 || index >= count() || index == m_index)
        return;
    m_index = index;
    emit changed();
    emit currentImageChanged();
}

QString History::name(int i) const
{
    return (i >= 0 && i < count()) ? m_entries[i].name : QString();
}

History::ImagePtr History::current() const
{
    return (m_index >= 0 && m_index < count()) ? m_entries[m_index].image : ImagePtr();
}

void History::setLimits(int maxEntries, qint64 maxBytes)
{
    m_maxEntries = std::max(2, maxEntries);
    m_maxBytes = maxBytes;
    trim();
    emit changed();
}
