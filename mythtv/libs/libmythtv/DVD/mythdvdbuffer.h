#ifndef MYTH_DVD_BUFFER_H_
#define MYTH_DVD_BUFFER_H_

// Qt
#include <QMap>
#include <QString>
#include <QMutex>
#include <QRect>
#include <QCoreApplication>

// MythTV
#include "io/mythopticalbuffer.h"
#include "mythdate.h"
#include "referencecounter.h"
#include "mythdvdcontext.h"
#include "mythdvdinfo.h"

// FFmpeg
extern "C" {
#include "libavcodec/avcodec.h"
}

#define DVD_BLOCK_SIZE 2048LL
#define DVD_MENU_MAX 7

class MythDVDPlayer;

class MTV_PUBLIC MythDVDBuffer : public MythOpticalBuffer
{
    Q_DECLARE_TR_FUNCTIONS(MythDVDBuffer)

  public:
    explicit MythDVDBuffer(const QString &Filename);
    ~MythDVDBuffer() override;

    bool      IsOpen               (void) const override;
    bool      IsBookmarkAllowed    (void) override;
    bool      IsSeekingAllowed     (void) override;
    int       BestBufferSize       (void) override { return 2048; }
    bool      IsInStillFrame       (void) const override;
    bool      OpenFile             (const QString &Filename,
                                    uint Retry = static_cast<uint>(kDefaultOpenTimeout)) override;
    bool      HandleAction         (const QStringList &Actions, int64_t Pts) override;
    void      IgnoreWaitStates     (bool Ignore) override;
    bool      StartFromBeginning   (void) override;
    long long GetReadPosition      (void) const override;
    bool      GetNameAndSerialNum  (QString& Name, QString& SerialNumber) override;

    int       GetTitle             (void) const;
    bool      DVDWaitingForPlayer  (void);
    int       GetPart              (void) const;
    int       GetCurrentAngle      (void) const;
    int       GetNumAngles         (void);
    long long GetTotalReadPosition (void);
    uint      GetChapterLength     (void) const;
    void      GetChapterTimes      (QList<long long> &Times);
    uint64_t  GetChapterTimes      (int Title);
    void      GetDescForPos        (QString &Description);
    void      GetPartAndTitle      (int &Part, int &Title) const;
    uint      GetTotalTimeOfTitle  (void);
    float     GetAspectOverride    (void);
    uint      GetCellStart         (void);
    bool      PGCLengthChanged     (void);
    bool      CellChanged          (void);
    bool      IsStillFramePending  (void) const;
    bool      AudioStreamsChanged  (void);
    bool      IsWaiting            (void) const;
    int       NumPartsInTitle      (void) const;
    void      GetMenuSPUPkt        (uint8_t *Buffer, int Size, int StreamID, uint32_t StartTime);
    uint32_t  AdjustTimestamp      (uint32_t Timestamp);
    int64_t   AdjustTimestamp      (int64_t  Timestamp);
    MythDVDContext* GetDVDContext  (void);
    int32_t   GetLastEvent         (void) const;
    AVSubtitle* GetMenuSubtitle    (uint &Version);
    int       NumMenuButtons       (void) const;
    QRect     GetButtonCoords      (void);
    void      ReleaseMenuButton    (void);
    uint      GetSubtitleLanguage  (int Id);
    int       GetSubtitleTrackNum  (uint StreamId);
    bool      DecodeSubtitles      (AVSubtitle* Subtitle, int* GotSubtitles,
                                    const uint8_t* SpuPkt, int BufSize, uint32_t StartTime);
    uint      GetAudioLanguage     (int Index);
    int       GetAudioTrackNum     (uint StreamId);
    int       GetAudioTrackType    (uint Index);
    bool      GetDVDStateSnapshot  (QString& State);
    bool      RestoreDVDStateSnapshot(QString& State);
    double    GetFrameRate         (void);
    bool      StartOfTitle         (void);
    bool      EndOfTitle           (void);
    void      PlayTitleAndPart     (int Title, int Part);
    void      CloseDVD             (void);
    bool      PlayTrack            (int Track);
    bool      NextTrack            (void);
    void      PrevTrack            (void);
    long long NormalSeek           (long long Time);
    bool      SectorSeek           (uint64_t Sector);
    void      SkipStillFrame       (void);
    void      WaitSkip             (void);
    void      SkipDVDWaitingForPlayer(void);
    void      UnblockReading       (void);
    bool      IsReadingBlocked     (void);
    bool      GoToMenu             (const QString &str);
    void      GoToNextProgram      (void);
    void      GoToPreviousProgram  (void);
    bool      GoBack               (void);
    void      AudioStreamsChanged  (bool Change);
    int64_t   GetCurrentTime       (void);
    uint      TitleTimeLeft        (void);
    void      SetTrack             (uint Type, int TrackNo);
    int       GetTrack             (uint Type);
    uint16_t  GetNumAudioChannels  (int Index);
    void      SetDVDSpeed          (void);
    void      SetDVDSpeed          (int Speed);
    bool      SwitchAngle          (int Angle);
    void      SetParent            (MythDVDPlayer *Parent);

  protected:
    int       SafeRead          (void *Buffer, uint Size) override;
    long long SeekInternal      (long long Position, int Whence) override;

    void      ActivateButton    (void);
    void      MoveButtonLeft    (void);
    void      MoveButtonRight   (void);
    void      MoveButtonUp      (void);
    void      MoveButtonDown    (void);
    bool      DVDButtonUpdate   (bool ButtonMode);
    void      ClearMenuSPUParameters (void);
    void      ClearMenuButton   (void);
    long long Seek              (long long Time);
    void      ClearChapterCache (void);
    void      SelectDefaultButton(void);
    void      WaitForPlayer     (void);

    static uint ConvertLangCode (uint16_t Code);
    static uint GetNibble       (const uint8_t *Buffer, int NibbleOffset);
    static int DecodeRLE        (uint8_t *Bitmap, int Linesize, int Width, int Height,
                                 const uint8_t *Buffer, int NibbleOffset, int BufferSize);
    void       GuessPalette     (uint32_t *RGBAPalette, const uint8_t *Palette, const uint8_t *Alpha);
    static int IsTransparent    (const uint8_t *Buffer, int Pitch, int Num,
                                 const uint8_t *Colors);
    static int FindSmallestBoundingRectangle(AVSubtitle *Subtitle);

    dvdnav_t      *m_dvdnav                 { nullptr };
    unsigned char  m_dvdBlockWriteBuf[DVD_BLOCK_SIZE] { 0 };
    unsigned char *m_dvdBlockReadBuf        { nullptr };
    int            m_dvdBlockRPos           { 0       };
    int            m_dvdBlockWPos           { 0       };
    long long      m_pgLength               { 0       };
    long long      m_pgcLength              { 0       };
    long long      m_cellStart              { 0       };
    bool           m_cellChanged            { false   };
    bool           m_pgcLengthChanged       { false   };
    long long      m_pgStart                { 0       };
    long long      m_currentpos             { 0       };
    int32_t        m_part                   { 0       };
    int32_t        m_lastPart               { 0       };
    int32_t        m_title                  { 0       };
    int32_t        m_lastTitle              { 0       };
    bool           m_playerWait             { false   };
    int32_t        m_titleParts             { 0       };
    bool           m_gotStop                { false   };
    int            m_currentTitleAngleCount { 0       };
    int64_t        m_endPts                 { 0       };
    int64_t        m_timeDiff               { 0       };
    int            m_still                  { 0       };
    int            m_lastStill              { 0       };
    bool           m_audioStreamsChanged    { false   };
    bool           m_dvdWaiting             { false   };
    long long      m_titleLength            { 0       };
    bool           m_skipstillorwait        { true    };
    long long      m_cellstartPos           { 0       };
    bool           m_buttonSelected         { false   };
    bool           m_buttonExists           { false   };
    bool           m_buttonSeenInCell       { false   };
    bool           m_lastButtonSeenInCell   { false   };
    int            m_cellid                 { 0       };
    int            m_lastcellid             { 0       };
    int            m_vobid                  { 0       };
    int            m_lastvobid              { 0       };
    bool           m_cellRepeated           { false   };
    int            m_curAudioTrack          { 0       };
    int8_t         m_curSubtitleTrack       { 0       };
    bool           m_autoselectsubtitle     { true    };
    bool           m_seeking                { false   };
    int64_t        m_seektime               { 0       };
    int64_t        m_currentTime            { 0       };
    QMap<int, int> m_seekSpeedMap;
    QMap<int, QList<uint64_t> > m_chapterMap;
    MythDVDPlayer  *m_parent                { nullptr };
    float           m_forcedAspect          { -1.0F   };
    QMutex          m_contextLock           { QMutex::Recursive };
    MythDVDContext *m_context               { nullptr };
    dvdnav_status_t m_dvdStat               { DVDNAV_STATUS_OK };
    int32_t         m_dvdEvent              { 0       };
    int32_t         m_dvdEventSize          { 0       };
    uint            m_buttonVersion         { 1       };
    int             m_buttonStreamID        { 0       };
    uint32_t        m_clut[16]              { 0       };
    uint8_t         m_button_color[4]       { 0       };
    uint8_t         m_button_alpha[4]       { 0       };
    QRect           m_hl_button             { 0, 0, 0, 0 };
    uint8_t        *m_menuSpuPkt            { nullptr };
    int             m_menuBuflength         { 0       };
    AVSubtitle      m_dvdMenuButton         {         };
    QMutex          m_menuBtnLock;
    QMutex          m_seekLock;
};

#endif
