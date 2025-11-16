#include "../Elem/WndText.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndShowAnimate.h"


class KUiRankLienTram : KWndShowAnimate {
public:

public:
    static KUiRankLienTram *OpenWindow(int nLienTram);

    static void CloseWindow(bool bDestory = true);

    static KUiRankLienTram *GetIfVisible();

private:
    static KUiRankLienTram *m_pSelf;
private:
    KUiRankLienTram();

    ~KUiRankLienTram();

    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

    void Initialize();

    void LoadScheme(const char *pScheme);

    void Breathe();
    void PaintWindow();
    
    void UpdateLienTram(int nLienTram);
    
private:
    KWndImage m_ImageLienTram1;  // Hundreds or single digit
    KWndImage m_ImageLienTram2;  // Tens digit  
    KWndImage m_ImageLienTram3;  // Units digit
    int m_nTimeout;
    int m_nLoopFrame;
    int m_nCurrentLienTram;
};
