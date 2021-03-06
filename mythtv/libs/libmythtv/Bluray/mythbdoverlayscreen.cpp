// Qt
#include <QPainter>

// MythTV
#include "mythuiimage.h"
#include "mythpainter.h"
#include "Bluray/mythbdbuffer.h"
#include "Bluray/mythbdoverlayscreen.h"

#define LOC QString("BDScreen: ")

MythBDOverlayScreen::MythBDOverlayScreen(MythPlayer *Player, const QString &Name)
  : MythScreenType(static_cast<MythScreenType*>(nullptr), Name),
    m_player(Player)
{
}

MythBDOverlayScreen::~MythBDOverlayScreen()
{
}

void MythBDOverlayScreen::DisplayBDOverlay(MythBDOverlay *Overlay)
{
    if (!Overlay || !m_player)
        return;

    MythRect rect(Overlay->m_x, Overlay->m_y, Overlay->m_image.width(), Overlay->m_image.height());
    SetArea(rect);
    DeleteAllChildren();

    MythVideoOutput *vo = m_player->GetVideoOutput();
    if (!vo)
        return;

    QImage& img = Overlay->m_image;

    // add to screen
    QRect scaled = vo->GetImageRect(rect);
    if (scaled.size() != rect.size())
        img = img.scaled(scaled.width(), scaled.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    MythPainter *osd_painter = vo->GetOSDPainter();
    if (osd_painter)
    {
        MythImage* image = osd_painter->GetFormatImage();
        if (image)
        {
            image->Assign(img);
            auto *uiimage = new MythUIImage(this, "bdoverlay");
            if (uiimage)
            {
                uiimage->SetImage(image);
                uiimage->SetArea(MythRect(scaled));
            }
            image->DecrRef();
        }
    }

    SetRedraw();
    delete Overlay;
}
