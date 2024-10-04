#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <QObject>
#include <QPixmap>
#include <QVector>

class SpriteSheet : public QObject {
    Q_OBJECT

public:
    explicit SpriteSheet(QObject *parent = nullptr);

    // Load the sprite sheet image
    void setSource(const QString &file_path);
    const QPixmap& source() const { return m_source; }

    // Setters and getters for orientation and cells
    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const { return m_orientation; }

    void setCells(int cells);
    int cells() const { return m_cells; }

    // Get the list of cropped images
    const QVector<QPixmap>& croppedImages();

private:
    QPixmap m_source;
    Qt::Orientation m_orientation;
    int m_cells;
    QVector<QPixmap> m_croppedImages;

    void generateCroppedImages();    
};

#endif