#ifndef HISTORY_H
#define HISTORY_H

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

#include "Image.h"

/// Snapshot based undo/redo history. Each entry owns an immutable image state.
class History : public QObject
{
    Q_OBJECT
public:
    explicit History(QObject *parent = nullptr);

    using ImagePtr = std::shared_ptr<const Image>;

    void reset(const QString &name, ImagePtr image);
    void push(const QString &name, ImagePtr image);
    void clear();

    bool canUndo() const { return m_index > 0; }
    bool canRedo() const { return m_index + 1 < static_cast<int>(m_entries.size()); }
    void undo();
    void redo();
    void jumpTo(int index);

    int count() const { return static_cast<int>(m_entries.size()); }
    int index() const { return m_index; }
    QString name(int i) const;
    ImagePtr current() const;

    void setLimits(int maxEntries, qint64 maxBytes);

signals:
    void changed();                 ///< list contents or index changed
    void currentImageChanged();     ///< the active image differs now

private:
    struct Entry {
        QString name;
        ImagePtr image;
    };
    void trim();
    static qint64 bytesOf(const ImagePtr &p);

    std::vector<Entry> m_entries;
    int m_index = -1;
    int m_maxEntries = 60;
    qint64 m_maxBytes = qint64(1536) * 1024 * 1024;
};

#endif // HISTORY_H
