#include "newgamedialog.h"
#include "filesystem.h"
#include "gameui_interface.h"
#include "gameui_util.h"
#include "vgui/ISurface.h"
#include "engineinterface.h"
#include "basepanel.h"

static const int MAX_CHAPTERS = 32;
struct chapter_t
{
    char filename[32];
};

float g_ScrollSpeedFast = 0.f;
float g_ScrollSpeedSlow = 0.f;

static int __cdecl ChapterSortFunc(const void* elem1, const void* elem2)
{
    chapter_t* c1 = (chapter_t*)elem1;
    chapter_t* c2 = (chapter_t*)elem2;

    // compare chapter number first
    static int chapterlen = strlen("chapter");
    if (atoi(c1->filename + chapterlen) > atoi(c2->filename + chapterlen))
        return 1;
    else if (atoi(c1->filename + chapterlen) < atoi(c2->filename + chapterlen))
        return -1;

    // compare length second ( longer string show up later in the list, eg. chapter9 before chapter9a )
    if (strlen(c1->filename) > strlen(c2->filename))
        return 1;
    else if (strlen(c1->filename) < strlen(c2->filename))
        return -1;

    // compare strings third
    return strcmp(c1->filename, c2->filename);
}

static int FindChapters(chapter_t* pChapters)
{
    int chapterIndex = 0;
    char szFullFileName[MAX_PATH];

    FileFindHandle_t findHandle = FILESYSTEM_INVALID_FIND_HANDLE;
    const char* fileName = "cfg/chapter*.cfg";
    fileName = g_pFullFileSystem->FindFirst(fileName, &findHandle);
    while (fileName && chapterIndex < MAX_CHAPTERS)
    {
        if (fileName[0])
        {
            // Only load chapter configs from the current mod's cfg dir
            // or else chapters appear that we don't want!
            Q_snprintf(szFullFileName, sizeof(szFullFileName), "cfg/%s", fileName);
            FileHandle_t f = g_pFullFileSystem->Open(szFullFileName, "rb", "MOD");
            if (f)
            {
                // don't load chapter files that are empty, used in the demo
                if (g_pFullFileSystem->Size(f) > 0)
                {
                    Q_strncpy(pChapters[chapterIndex].filename, fileName, sizeof(pChapters[chapterIndex].filename));
                    ++chapterIndex;
                }
                g_pFullFileSystem->Close(f);
            }
        }
        fileName = g_pFullFileSystem->FindNext(findHandle);
    }

    qsort(pChapters, chapterIndex, sizeof(chapter_t), &ChapterSortFunc);
    return chapterIndex;
}
/* End from GameUI */

static int GetUnlockedChapters()
{
    ConVarRef var("sv_unlockedchapters");

    return var.IsValid() ? MAX(var.GetInt(), 1) : 1;
}

CNewGameDialog::CNewGameDialog(vgui::Panel* parent) : vgui::Frame(parent, "NewGameDialog")
{
    
}

CNewGameDialog::~CNewGameDialog()
{
}

void CNewGameDialog::Activate()
{
}

void CNewGameDialog::ApplySettings(KeyValues* inResourceData)
{
    BaseClass::ApplySettings(inResourceData);

    int iChapterYPos = inResourceData->GetInt("chapterypos", 40);
    m_flPanelYPos[0] = iChapterYPos;
    m_flPanelYPos[1] = iChapterYPos;
    m_flPanelYPos[2] = iChapterYPos;
    m_flPanelYPos[3] = iChapterYPos;
    m_flPanelYPos[4] = iChapterYPos;
    m_pBGPanel->SetTall(inResourceData->GetInt("centerbgtall", 0));
	g_ScrollSpeedSlow = inResourceData->GetFloat("scrollslow", 0.0);
	g_ScrollSpeedFast = inResourceData->GetFloat("scrollfast", 0.0);
    m_flScrollSpeed = g_ScrollSpeedSlow;
    return;
}

void CNewGameDialog::OnCommand(const char* command)
{
    // todo reduce duplicated code
    if (V_stricmp(command, "Play"))
    {
        if (m_bSomething2 != false) {
            return;
        }
        StartGame();
    }
    else if (!V_stricmp(command, "Next")) 
    {
        if (m_bSomething2 != false) {
            return;
        }
        if (m_bSomething0 != 0) {
            return;
        }
        if (m_iPanelSomething[1] != -1) {
            m_vecChapterPanels[m_iPanelSomething[1]]->SetZPos(0);
        }
        m_bSomething1 = (bool)(m_bSomething1 ^ 1);
        m_eScrollDir2 = m_eScrollDir2 + 1;

        m_bSomething0 = 1;
        AnimateSelectionPanels();
        UpdateMenuComponents(m_eScrollDir1);
    }
    else if (!V_stricmp(command, "Prev"))
    {
        if (m_bSomething2 != false) {
            return;
        }
        if (m_bSomething0 != 0) {
            return;
        }
        if (m_iPanelSomething[3] != -1)
        {
            m_vecChapterPanels[m_iPanelSomething[3]]->SetZPos(0);
        }
        m_bSomething1 = (bool)(m_bSomething1 ^ 1);
        if (m_iPanelSomething[2] != 0) {
            m_eScrollDir2 = m_eScrollDir2 - 1;
        }

        m_bSomething0 = 1;
        AnimateSelectionPanels();
        UpdateMenuComponents(m_eScrollDir1);
    }
    else if (!V_stricmp(command, "Mode_Next"))
    {
        if (m_bSomething2 != false) {
            return;
        }
        if (m_iVar03 == 0) {
            return;
        }
        m_iVar01 = m_iVar01 + 1;

        vgui::surface()->PlaySound("UI/buttonclick.wav");
        UpdateBonusSelection();
    }
    else if (!V_stricmp(command, "Mode_Prev"))
    {
        if (m_bSomething2 != false) {
            return;
        }
        if (m_iVar03 == 0) {
            return;
        }
        m_iVar01 = m_iVar01 + -1;

        vgui::surface()->PlaySound("UI/buttonclick.wav");
        UpdateBonusSelection();
    }
    else if (!V_stricmp(command, "ReleaseModalWindow"))
    {
        vgui::surface()->RestrictPaintToSinglePanel(NULL);
        m_pKeyRepeatHandler = nullptr;
        //*(undefined*)&m_huh = 0;
    }
    else 
    {
        BaseClass::OnCommand(command);
        m_pKeyRepeatHandler = nullptr;
        //*(undefined*)&m_huh = 0;
    }
}

void CNewGameDialog::ApplySchemeSettings(vgui::IScheme* pScheme)
{
}

void CNewGameDialog::OnKeyCodeTyped(vgui::KeyCode code)
{
}

void CNewGameDialog::OnKeyCodePressed(vgui::KeyCode code)
{
}

void CNewGameDialog::OnKeyCodeReleased(vgui::KeyCode code)
{
	m_pKeyRepeatHandler->KeyUp(code);
	BaseClass::OnKeyCodeReleased(code);
}

void CNewGameDialog::OnThink()
{
	ButtonCode_t code = m_pKeyRepeatHandler->KeyRepeated();
    if(code)
	{
		OnKeyCodeTyped(code);
	}
	BaseClass::OnThink();
}

void CNewGameDialog::AnimateSelectionPanels()
{
}

void CNewGameDialog::ContinueScrolling()
{
}

void CNewGameDialog::PostScroll(EScrollDirection scrollDir)
{
}

void CNewGameDialog::ScrollSelectionPanels(EScrollDirection scrollDir)
{
}

void CNewGameDialog::SetSelectedChapter(const char* name)
{
}

void CNewGameDialog::SetSelectedChapterIndex(int index)
{
}

void CNewGameDialog::ShiftPanelIndices(int index)
{
}

void CNewGameDialog::StartGame()
{
    if ((-1 < m_iDesiredChapter) && (m_iDesiredChapter < m_vecChapterPanels.Size()))
    {
        char szCommand[512];
        szCommand[0] = '\0';
        V_snprintf(szCommand, 512, "disconnect\ndeathmatch 0\nprogress_enable\nexec %s\n", m_vecChapterPanels[m_iDesiredChapter]->GetName());
		ConVarRef commentary("commentary");
		commentary.SetValue(m_bIsCommentary);
		ConVarRef m_bIsCheats("sv_cheats");
		m_bIsCheats.SetValue(m_bIsCommentary);

        if (!m_bIsCommentary /*|| m_vecChapterPanels[m_iDesiredChapter] + 0x59c) != '\0'*/)
        {
            //BasePanel()->FadeToBlackAndRunEngineCommand(szCommand);
			engine->ExecuteClientCmd(szCommand);
        }
        else 
        {
            if (GameUI().IsInMultiplayer()) 
            {
                engine->ExecuteClientCmd("disconnect");
            }
            //if (!m_hCommentaryDialog.Get())
            //    m_hCommentaryDialog = new CCommentaryExplanationDialog(BasePanel(), szCommand);
            //
            //m_hCommentaryDialog->DoModal();
            engine->ExecuteClientCmd(szCommand);
        }
        OnClose();
    }
}

void CNewGameDialog::UpdateBonusSelection()
{
}

void CNewGameDialog::UpdateMenuComponents(EScrollDirection scrollDir)
{
}
