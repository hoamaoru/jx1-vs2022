#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "UiRankLienTram.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"
//#include ".../Config/GameSettings.h"


extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;
#define        SCHEME_INI            "UiRankLienTram.ini"

KUiRankLienTram *KUiRankLienTram::m_pSelf = NULL;

KUiRankLienTram::KUiRankLienTram() {
}

KUiRankLienTram::~KUiRankLienTram() {
}

KUiRankLienTram *KUiRankLienTram::OpenWindow(int nLienTram) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiRankLienTram;
        m_pSelf->Initialize();
    }
    if (m_pSelf) {
        m_pSelf->Show();
        m_pSelf->BringToTop();
        m_pSelf->UpdateLienTram(nLienTram);
    }
    return m_pSelf;
}


void KUiRankLienTram::CloseWindow(bool bDestory) {
    if (m_pSelf) {
        m_pSelf->Hide();
        if (bDestory) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }
    }
}

KUiRankLienTram *KUiRankLienTram::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    else
        return NULL;
}

void KUiRankLienTram::Initialize() {
    m_nTimeout = 0;
    m_nLoopFrame = 0;
    AddChild(&m_ImageLienTram1);
    AddChild(&m_ImageLienTram2);
    AddChild(&m_ImageLienTram3);  // Add third image child
    Wnd_AddWindow(this);

    char szScheme[128];
    g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
    LoadScheme(szScheme);
}

void KUiRankLienTram::LoadScheme(const char *pScheme) {
    if (m_pSelf && pScheme) {
        char Buff[128];
        KIniFile Ini;
        sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
        if (Ini.Load(Buff))
        {
            //if (WND_SCREEN_WIDTH != 1024)
            //    Ini.SetKeyValue("Main", "Left", "600");
            Init(&Ini, "Main");
            m_ImageLienTram1.Init(&Ini, "ImageLienTram1");
            m_ImageLienTram2.Init(&Ini, "ImageLienTram2");
            m_ImageLienTram3.Init(&Ini, "ImageLienTram3");  // Initialize third image
        }
    }
}

void KUiRankLienTram::UpdateLienTram(int nLienTram) {
    if (nLienTram == 0)
    {
        m_nTimeout = 0;
        m_nCurrentLienTram = 0;
        this->CloseWindow(false);
    }
    else
    {
        m_nCurrentLienTram = nLienTram;
        m_nTimeout = 90;
        char szBuffer[128];

        if (m_nCurrentLienTram < 10)
        {
            // Single digit: only show m_ImageLienTram1
            m_ImageLienTram2.Hide();
            m_ImageLienTram3.Hide();
            sprintf(szBuffer, "\\spr\\vlcc\\lientram\\%d.spr", m_nCurrentLienTram);
            m_ImageLienTram1.SetImage(ISI_T_SPR, szBuffer, true);
        }
        else if (m_nCurrentLienTram < 100)
        {
            // Two digits: show m_ImageLienTram1 (tens) and m_ImageLienTram2 (units)
            m_ImageLienTram3.Hide();
            if (!m_ImageLienTram2.IsVisible())
                m_ImageLienTram2.Show();
            
            sprintf(szBuffer, "\\spr\\vlcc\\lientram\\%d.spr", m_nCurrentLienTram / 10);
            m_ImageLienTram1.SetImage(ISI_T_SPR, szBuffer, true);
            
            sprintf(szBuffer, "\\spr\\vlcc\\lientram\\%d.spr", m_nCurrentLienTram % 10);
            m_ImageLienTram2.SetImage(ISI_T_SPR, szBuffer, true);
        }
        else
        {
            // Three digits: show all three images
            if (m_nCurrentLienTram > 999) m_nCurrentLienTram = 999;  // Cap at 999
            
            if (!m_ImageLienTram2.IsVisible())
                m_ImageLienTram2.Show();
            if (!m_ImageLienTram3.IsVisible())
                m_ImageLienTram3.Show();
            
            // Hundreds digit
            sprintf(szBuffer, "\\spr\\vlcc\\lientram\\%d.spr", m_nCurrentLienTram / 100);
            m_ImageLienTram1.SetImage(ISI_T_SPR, szBuffer, true);
            
            // Tens digit  
            sprintf(szBuffer, "\\spr\\vlcc\\lientram\\%d.spr", (m_nCurrentLienTram / 10) % 10);
            m_ImageLienTram2.SetImage(ISI_T_SPR, szBuffer, true);
            
            // Units digit
            sprintf(szBuffer, "\\spr\\vlcc\\lientram\\%d.spr", m_nCurrentLienTram % 10);
            m_ImageLienTram3.SetImage(ISI_T_SPR, szBuffer, true);
        }
        
        if (!this->IsVisible())
        {
            this->Show();
            this->BringToTop();
        }
    }
}

void KUiRankLienTram::Breathe() {
    if (m_nTimeout > 0)
    {
        m_nTimeout--;
        if (m_nTimeout == 0)
            this->CloseWindow(false);
    }
}

void KUiRankLienTram::PaintWindow() {
    KWndImage::PaintWindow();
    m_nLoopFrame++;
    if (m_nLoopFrame == 10 && this->IsVisible())
    {
        m_nLoopFrame = 0;
        NextFrame();
        m_ImageLienTram1.NextFrame();
        if (m_ImageLienTram2.IsVisible()) 
            m_ImageLienTram2.NextFrame();
        if (m_ImageLienTram3.IsVisible()) 
            m_ImageLienTram3.NextFrame();  // Animate third image if visible
    }
   
}

int KUiRankLienTram::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    return 0;
}
