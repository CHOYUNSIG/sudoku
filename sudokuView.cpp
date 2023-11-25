﻿
// sudokuView.cpp: CsudokuView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "sudoku.h"
#endif

#include "MainFrm.h"
#include "sudokuDoc.h"
#include "sudokuView.h"
#include "SudokuMap.h"
#include "CustomButton.h"
#include <algorithm>
#include <string.h>
#include <functional>
#include <direct.h>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CsudokuView

IMPLEMENT_DYNCREATE(CsudokuView, CView)

BEGIN_MESSAGE_MAP(CsudokuView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()




// CsudokuView 생성/소멸

CsudokuView::CsudokuView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CsudokuView::~CsudokuView()
{
}

BOOL CsudokuView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	return CView::PreCreateWindow(cs);
}

void CsudokuView::AfterCreateWindow()
{	
	OnSoundVolumeClicked(0);
	OnScreenSizeClicked(0);
	OnLanguageClicked(0);
	group_init->Enable();
}

// CsudokuView 그리기

void CsudokuView::OnDraw(CDC *pDC)
{
	if (m_bAfterCreateWindow) {
		m_bAfterCreateWindow = false;
		AfterCreateWindow();
	}

	CsudokuDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	memdc.SetBkMode(TRANSPARENT);
	
	CRect ClientRect;
	GetClientRect(ClientRect);
	width = ClientRect.Width();
	height = ClientRect.Height();
	
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, width, height);
	memdc.SelectObject(bitmap);
	memdc.FillSolidRect(ClientRect, RGB(246, 236, 226));

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CFont font, *oldfont;
	CPen pen, *oldpen;
	CString string;

	if (m_mode == GAME) {
		// 게임판 뒷배경
		memdc.FillSolidRect(CRect(
			width / 20,
			height / 20,
			width / 20 + height * 9 / 10,
			height * 19 / 20
		), RGB(255, 255, 255));
		if (m_ingame == ON) {
			// 동일한 숫자의 셀 색칠
			if (m_map->GetValue(m_nSelRow, m_nSelCol) != 0)
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
						if (!(i == m_nSelRow && j == m_nSelCol) && m_map->GetValue(i, j) == m_map->GetValue(m_nSelRow, m_nSelCol))
							memdc.FillSolidRect(CRect(
								width / 20 + height * j / 10,
								height * (2 * i + 1) / 20,
								width / 20 + height * (j + 1) / 10,
								height * (2 * i + 3) / 20
							), RGB(220, 220, 220));
		}
	}

	// 버튼
	Button::Draw(&memdc); 

	if (m_mode == INIT) {
		// 제목
		{
			font.CreatePointFont((int)(height * 1.5 * 72 / GetDpiForWindow(GetSafeHwnd())), font_name);
			oldfont = memdc.SelectObject(&font);
			memdc.DrawText(_T(" Sudoku™"), CRect(0, height / 10, width, height * 3 / 10), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			memdc.SelectObject(oldfont);
			font.DeleteObject();
		}
		if (m_menu == START) {
			// 랭킹판
			{
				// 랭킹판 - 등수
				CString rank[5] = { CString("1st"), CString("2nd"), CString("3rd"), CString("4th"), CString("5th") };
				size_t max_ranked = max(max(ranking[0]->size(), ranking[1]->size()), ranking[2]->size());
				font.CreatePointFont((int)(height * 32 / GetDpiForWindow(GetSafeHwnd())), font_name);
				oldfont = memdc.SelectObject(&font);
				for (int i = 0; i < max_ranked; i++) {
					memdc.DrawText(rank[i], CRect(
						width / 2,
						height * (6 + i) / 12,
						width * 6 / 10,
						height * (7 + i) / 12
					), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
				memdc.SelectObject(oldfont);
				font.DeleteObject();
				// 랭킹판 - 난이도 별 기록
				CString diff[3] = { CString("쉬움"), CString("보통"), CString("어려움") };
				for (int d = 0; d < 3; d++) {
					if (ranking[d]->empty())
						continue;
					font.CreatePointFont((int)(height * 32 / GetDpiForWindow(GetSafeHwnd())), font_name);
					oldfont = memdc.SelectObject(&font);
					memdc.DrawText(diff[d], CRect(
						width * (6 + d) / 10,
						height * 5 / 12,
						width * (7 + d) / 10,
						height * 6 / 12
					), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					memdc.SelectObject(oldfont);
					font.DeleteObject();
					for (int i = 0; i < ranking[d]->size(); i++) {
						font.CreatePointFont((int)(height * 24 / GetDpiForWindow(GetSafeHwnd())), _T("consolas"));
						oldfont = memdc.SelectObject(&font);
						string.Format(_T("%.2fs"), ranking[d]->at(i));
						memdc.DrawText(string, CRect(
							width * (6 + d) / 10,
							height * (6 + i) / 12,
							width * (7 + d) / 10,
							height * (7 + i) / 12
						), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						memdc.SelectObject(oldfont);
						font.DeleteObject();
					}
				}
			}
		}
	}
	else if (m_mode == LOADING) {
		// 로딩
		{
			font.CreatePointFont((int)(height * 72 / GetDpiForWindow(GetSafeHwnd())), font_name);
			oldfont = memdc.SelectObject(&font);
			string = CString("Loading");
			for (int i = 0; i < (clock() - m_clockRequested) * 2 / CLOCKS_PER_SEC % 4; i++)
				string += CString(".");
			memdc.DrawText(string, CRect(0, 0, width, height), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			memdc.SelectObject(oldfont);
			font.DeleteObject();
		}
	}
	else if (m_mode == GAME) {
		if (m_ingame == READY) {
			// 카운트다운
			{
				font.CreatePointFont(height * 72 / GetDpiForWindow(GetSafeHwnd()), font_name);
				oldfont = memdc.SelectObject(&font);
				memdc.DrawText(_T("준비!"), CRect(width / 20, height / 20, width / 20 + height * 9 / 10, height / 2), DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
				memdc.SelectObject(oldfont);
				font.DeleteObject();
				font.CreatePointFont(height * 72 / GetDpiForWindow(GetSafeHwnd()), _T("consolas"));
				oldfont = memdc.SelectObject(&font);
				string.Format(_T("%.2fs"), 3 - (double)(clock() - m_clockGenerated) / CLOCKS_PER_SEC);
				memdc.DrawText(string, CRect(width / 20, height / 2, width / 20 + height * 9 / 10, height * 19 / 20), DT_SINGLELINE | DT_CENTER | DT_TOP);
				memdc.SelectObject(oldfont);
				font.DeleteObject();
			}
		}
		if (m_ingame == READY || m_ingame == PAUSE) {
			// 게임판 테두리
			{
				CPen pen, *oldpen;
				pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
				oldpen = memdc.SelectObject(&pen);
				memdc.MoveTo(CPoint(width / 20, height / 20));
				memdc.LineTo(CPoint(width / 20 + height * 9 / 10, height / 20));
				memdc.MoveTo(CPoint(width / 20, height / 20));
				memdc.LineTo(CPoint(width / 20, height * 19 / 20));
				memdc.MoveTo(CPoint(width / 20, height * 19 / 20));
				memdc.LineTo(CPoint(width / 20 + height * 9 / 10, height * 19 / 20));
				memdc.MoveTo(CPoint(width / 20 + height * 9 / 10, height / 20));
				memdc.LineTo(CPoint(width / 20 + height * 9 / 10, height * 19 / 20));
				memdc.SelectObject(oldpen);
				pen.DeleteObject();
			}
		}
		if (m_ingame == ON || m_ingame == DONE) {
			// 게임판
			{
				// 게임판 - 메모 숫자
				font.CreatePointFont(height * 24 / GetDpiForWindow(GetSafeHwnd()), _T("굴림"));
				oldfont = memdc.SelectObject(&font);
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++) {
						if (m_map->GetValue(i, j) == 0) {
							int memo = m_map->GetMemo(i, j);
							for (int k = 0; k < 9; k++)
								if (memo & (1 << k)) {
									string.Format(_T("%d"), k + 1);
									if (k + 1 == m_map->GetValue(m_nSelRow, m_nSelCol))
										memdc.SetTextColor(RGB(0, 0, 0));
									else
										memdc.SetTextColor(RGB(180, 180, 180));
									memdc.DrawText(string, CRect(
										width / 20 + height * j / 10 + height * (k % 3) / 30,
										height * (i * 2 + 1) / 20 + height * (k / 3) / 30,
										width / 20 + height * j / 10 + height * (k % 3 + 1) / 30,
										height * (i * 2 + 1) / 20 + height * (k / 3 + 1) / 30
									), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
								}
						}
					}
				memdc.SetTextColor(RGB(0, 0, 0));
				memdc.SelectObject(oldfont);
				font.DeleteObject();
				// 게임판 - 가는 선
				pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				oldpen = memdc.SelectObject(&pen);
				for (int i = 0; i < 10; i++) {
					memdc.MoveTo(CPoint(width / 20, height * (2 * i + 1) / 20));
					memdc.LineTo(CPoint(width / 20 + height * 9 / 10, height * (2 * i + 1) / 20));
					memdc.MoveTo(CPoint(width / 20 + height * i / 10, height / 20));
					memdc.LineTo(CPoint(width / 20 + height * i / 10, height * 19 / 20));
				}
				memdc.SelectObject(oldpen);
				pen.DeleteObject();
				// 게임판 - 굵은 선
				pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
				oldpen = memdc.SelectObject(&pen);
				for (int i = 0; i < 4; i++) {
					memdc.MoveTo(CPoint(width / 20, height * (6 * i + 1) / 20));
					memdc.LineTo(CPoint(width / 20 + height * 9 / 10, height * (6 * i + 1) / 20));
					memdc.MoveTo(CPoint(width / 20 + height * i * 3 / 10, height / 20));
					memdc.LineTo(CPoint(width / 20 + height * i * 3 / 10, height * 19 / 20));
				}
				memdc.SelectObject(oldpen);
				pen.DeleteObject();
				// 게임판 - 선택된 셀
				pen.CreatePen(PS_SOLID, 5, RGB(255, 100, 100));
				oldpen = memdc.SelectObject(&pen);
				memdc.MoveTo(CPoint(width / 20 + height * m_nSelCol / 10, height * (2 * m_nSelRow + 1) / 20));
				memdc.LineTo(CPoint(width / 20 + height * m_nSelCol / 10, height * (2 * m_nSelRow + 3) / 20));
				memdc.MoveTo(CPoint(width / 20 + height * (m_nSelCol + 1) / 10, height * (2 * m_nSelRow + 1) / 20));
				memdc.LineTo(CPoint(width / 20 + height * (m_nSelCol + 1) / 10, height * (2 * m_nSelRow + 3) / 20));
				memdc.MoveTo(CPoint(width / 20 + height * m_nSelCol / 10, height * (2 * m_nSelRow + 1) / 20));
				memdc.LineTo(CPoint(width / 20 + height * (m_nSelCol + 1) / 10, height * (2 * m_nSelRow + 1) / 20));
				memdc.MoveTo(CPoint(width / 20 + height * m_nSelCol / 10, height * (2 * m_nSelRow + 3) / 20));
				memdc.LineTo(CPoint(width / 20 + height * (m_nSelCol + 1) / 10, height * (2 * m_nSelRow + 3) / 20));
				memdc.SelectObject(oldpen);
				pen.DeleteObject();
			}
		}
		if (m_ingame == ON) {
			// 경과 시간
			{
				font.CreatePointFont(height * 36 / GetDpiForWindow(GetSafeHwnd()), _T("consolas"));
				oldfont = memdc.SelectObject(&font);
				string.Format(_T("%.2fs"), (double)(clock() - m_clockStarted) / CLOCKS_PER_SEC);
				memdc.DrawText(string, CRect(
					0,
					height / 9,
					width * 21 / 40 + height * 9 / 20,
					height * 2 / 9
				), DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
				memdc.SelectObject(oldfont);
				font.DeleteObject();
			}
		}
		if (m_ingame == DONE) {
			// 게임 결과
			{
				// 게임 결과 - 난이도
				font.CreatePointFont(height * 36 / GetDpiForWindow(GetSafeHwnd()), font_name);
				oldfont = memdc.SelectObject(&font);
				switch (m_diff) {
				case EASY:
					string = CString("쉬움");
					break;
				case MEDIUM:
					string = CString("보통");
					break;
				case HARD:
					string = CString("어려움");
					break;
				case USER:
					string = CString("사용자 정의");
					break;
				}
				memdc.DrawText(string, CRect(
					width * 21 / 40 + height * 9 / 20 - width,
					0,
					width * 21 / 40 + height * 9 / 20 + width,
					height / 2
				), DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
				memdc.SelectObject(oldfont);
				font.DeleteObject();
				// 게임 결과 - 완료 시간
				font.CreatePointFont(height * 72 / GetDpiForWindow(GetSafeHwnd()), _T("consolas"));
				oldfont = memdc.SelectObject(&font);
				string.Format(_T("%.2fs"), (double)(m_clockEnded - m_clockStarted) / CLOCKS_PER_SEC);
				memdc.DrawText(string, CRect(
					width * 21 / 40 + height * 9 / 20 - width,
					height / 2,
					width * 21 / 40 + height * 9 / 20 + width,
					height
				), DT_SINGLELINE | DT_CENTER | DT_TOP);
				memdc.SelectObject(oldfont);
				font.DeleteObject();
			}
		}
	}

	pDC->BitBlt(0, 0, ClientRect.Width(), ClientRect.Height(), &memdc, 0, 0, SRCCOPY);
	memdc.DeleteDC();
}


// CsudokuView 인쇄

BOOL CsudokuView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CsudokuView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CsudokuView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CsudokuView 진단

#ifdef _DEBUG
void CsudokuView::AssertValid() const
{
	CView::AssertValid();
}

void CsudokuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CsudokuDoc* CsudokuView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CsudokuDoc)));
	return (CsudokuDoc*)m_pDocument;
}
#endif //_DEBUG


// CsudokuView 메시지 처리기


int CsudokuView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	// 화면 주사율 조정
	SetTimer(0, 500 / FPS, NULL);
	
	// 폰트 로딩
	{
		char font_path[256] = { 0, };
		if (_getcwd(font_path, sizeof(font_path)) != nullptr) {
			strcat_s(font_path, "\\res\\MaruBuri-Regular.ttf");
			AddFontResource(CString(font_path));
		}

		TextButton::m_nDPI = GetDpiForWindow(GetSafeHwnd());
		font_name = CString("마루 부리 중간");
	}

	// 설정값 복원
	{
		CStdioFile preset;
		preset.Open(_T("settings.dat"), CFile::modeRead | CFile::typeText);
		char buffer[1024] = { 0, };
		preset.Read(buffer, sizeof(buffer));
		preset.Close();

		char *context = nullptr;
		char *line = strtok_s(buffer, ":", &context);
		int *pre_setting_value[3] = {
			&m_nSoundVolume,
			&m_nScreenRatio,
			&m_nLanguage
		};

		for (int i = 0; i < 3; i++) {
			line = strtok_s(NULL, "\n", &context);
			*pre_setting_value[i] = atoi(line);
			line = strtok_s(NULL, ":", &context);
		}
	}

	// 랭킹 복원
	{
		CStdioFile rank;
		rank.Open(_T("ranking.dat"), CFile::modeRead | CFile::typeText);
		char buffer[1024] = { 0, };
		rank.Read(buffer, sizeof(buffer));
		rank.Close();

		char *context = nullptr;
		char *line = strtok_s(buffer, ":", &context);

		for (int d = 0; d < 3; d++) {
			ranking[d] = new std::vector<double>;
			for (int i = 0; i < 5; i++) {
				line = strtok_s(NULL, "\n", &context);
				double sec = atof(line);
				if (sec > 0)
					ranking[d]->push_back(sec);
				line = strtok_s(NULL, ":", &context);
			}
		}
	}

	/*
	* 
	* 버튼 생성 단계
	* : 게임에서 사용할 버튼은 여기서 전부 선언합니다.
	* 
	*/
	
	// 메뉴
	{
		Corner menu_rect[5][2][2];
		Corner menu_sp[5][2];
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 2; j++) {
				menu_rect[i][j][0] = [=](int width, int height) { return CPoint(width * (1 + j * 5) / 10, height * 2 / 5 + height * i / 10); };
				menu_rect[i][j][1] = [=](int width, int height) { return CPoint(width * (4 + j * 5) / 10, height * 2 / 5 + height * (i + 1) / 10); };
				menu_sp[i][j] = [=](int width, int height) { return CPoint(width * (j * 15 - 4) / 10, height * 2 / 5 + height * i / 10); };
			}
		}

		// 메뉴 - INIT
		{
			Button *button_init[4] = { nullptr, };
			button_init[0] = new AnimationButton(menu_rect[0][0][0], menu_rect[0][0][1], [=]() { OnNewgameClicked(); }, CString("새 게임"), font_name, 0.5, menu_sp[0][0], 0.3, 0b11);
			button_init[1] = new AnimationButton(menu_rect[1][0][0], menu_rect[1][0][1], [=]() { OnContinueClicked(); }, CString("이어하기"), font_name, 0.5, menu_sp[1][0], 0.35, 0b11);
			button_init[2] = new AnimationButton(menu_rect[2][0][0], menu_rect[2][0][1], [=]() { OnSettingsClicked(); }, CString("설정"), font_name, 0.5, menu_sp[2][0], 0.4, 0b11);
			button_init[3] = new AnimationButton(menu_rect[3][0][0], menu_rect[3][0][1], [=]() { OnExitClicked(); }, CString("종료"), font_name, 0.5, menu_sp[3][0], 0.45, 0b11);
			group_init = new ButtonGroup(4, button_init);
		}

		// 메뉴 - NEWGAME
		{
			Button *button_newgame[5] = { nullptr, };
			button_newgame[0] = new AnimationButton(menu_rect[0][0][0], menu_rect[0][0][1], [=]() { OnDifficultyClicked(EASY); }, CString("쉬움"), font_name, 0.5, menu_sp[0][0], 0.3, 0b11);
			button_newgame[1] = new AnimationButton(menu_rect[1][0][0], menu_rect[1][0][1], [=]() { OnDifficultyClicked(MEDIUM); }, CString("보통"), font_name, 0.5, menu_sp[1][0], 0.35, 0b11);
			button_newgame[2] = new AnimationButton(menu_rect[2][0][0], menu_rect[2][0][1], [=]() { OnDifficultyClicked(HARD); }, CString("어려움"), font_name, 0.5, menu_sp[2][0], 0.4, 0b11);
			button_newgame[3] = new AnimationButton(menu_rect[3][0][0], menu_rect[3][0][1], [=]() { OnUserClicked(); }, CString("사용자 정의"), font_name, 0.5, menu_sp[3][0], 0.45, 0b11);
			button_newgame[4] = new AnimationButton(menu_rect[4][0][0], menu_rect[4][0][1], [=]() { OnBackNewGameClicked(); }, CString("뒤로"), font_name, 0.5, menu_sp[4][0], 0.5, 0b11);
			group_newgame = new ButtonGroup(5, button_newgame);
		}

		// 메뉴 - SETTINGS
		{
			Button *button_settings[13] = { nullptr, };

			// 메뉴 - SETTINGS - 항목
			button_settings[0] = new AnimationButton(menu_rect[0][0][0], menu_rect[0][0][1], [=]() {}, CString("음량"), font_name, 0.5, menu_sp[0][0], 0.3, 0b00);
			button_settings[1] = new AnimationButton(menu_rect[1][0][0], menu_rect[1][0][1], [=]() {}, CString("화면 크기"), font_name, 0.5, menu_sp[1][0], 0.35, 0b00);
			button_settings[2] = new AnimationButton(menu_rect[2][0][0], menu_rect[2][0][1], [=]() {}, CString("언어"), font_name, 0.5, menu_sp[2][0], 0.4, 0b00);

			// 메뉴 - SETTINGS - 뒤로
			button_settings[3] = new AnimationButton(menu_rect[3][0][0], menu_rect[3][0][1], [=]() { OnBackSettingsClicked(); }, CString("뒤로"), font_name, 0.5, menu_sp[3][0], 0.45, 0b11);

			// 메뉴 - SETTINGS - 설정값
			button_settings[4] = new AnimationButton(menu_rect[0][1][0], menu_rect[0][1][1], []() {}, CString(""), font_name, 0.5, menu_sp[0][1], 0.3, 0b00);
			button_settings[5] = new AnimationButton(menu_rect[1][1][0], menu_rect[1][1][1], []() {}, CString(""), font_name, 0.5, menu_sp[1][1], 0.35, 0b00);
			button_settings[6] = new AnimationButton(menu_rect[2][1][0], menu_rect[2][1][1], []() {}, CString(""), font_name, 0.5, menu_sp[2][1], 0.4, 0b00);

			// 메뉴 - SETTINGS - 설정값 조정
			std::function<void(int)> cb[3] = {
				[=](int inc) { OnSoundVolumeClicked(inc); },
				[=](int inc) { OnScreenSizeClicked(inc); },
				[=](int inc) { OnLanguageClicked(inc); }
			};

			for (int i = 0; i < 3; i++) {
				button_settings[7 + i * 2] = new AnimationButton(menu_rect[i][1][0],
					[=](int width, int height) {
						CPoint rect = menu_rect[i][1][0](width, height);
						return CPoint(rect.x + height / 10, rect.y + height / 10);
					}, [=]() { cb[i](-1); }, CString("◀"), font_name, 0.5, menu_sp[i][1], 0.3 + 0.05 * i, 0b00);
				button_settings[8 + i * 2] = new AnimationButton(
					[=](int width, int height) {
						CPoint rect = menu_rect[i][1][1](width, height);
						return CPoint(rect.x - height / 10, rect.y - height / 10);
					}, menu_rect[i][1][1], [=]() { cb[i](1); }, CString("▶"), font_name, 0.5, menu_sp[i][1], 0.3 + 0.05 * i, 0b00);
			}

			group_settings = new ButtonGroup(13, button_settings);
		}
	}

	// 게임
	{
		// 게임 - 숫자키
		{
			Button *button_number[9] = { nullptr, };
			for (int i = 0; i < 9; i++) {
				CString a;
				a.Format(_T("%d"), i + 1);
				Corner tl = [=](int width, int height) {
					return CPoint(
						width * 21 / 40 + height * 9 / 20 + height * (2 * (i % 3) - 3) / 18 + 3,
						height * (3 + i / 3) / 9 + 3
					);
					};
				Corner br = [=](int width, int height) {
					return CPoint(
						width * 21 / 40 + height * 9 / 20 + height * (2 * (i % 3) - 1) / 18 - 3,
						height * (4 + i / 3) / 9 - 3
					);
					};
				button_number[i] = new EdgeButton(tl, br, [=]() { OnNumberKeyClicked(i + 1); }, a, font_name, 0.5, 1, 0.1);
			}
			group_numberkey = new ButtonGroup(9, button_number);
		}

		// 게임 - 일시정지
		{
			Corner tl = [=](int width, int height) {
				return CPoint(
					width * 21 / 40 + height * 9 / 20 + height / 9,
					height / 9
				);
				};
			Corner br = [=](int width, int height) {
				return CPoint(
					width * 21 / 40 + height * 9 / 20 + height * 2 / 9,
					height * 2 / 9
				);
				};
			Button *button_pause = new EdgeButton(tl, br, []() {}, CString("∥"), CString("consolas"), 0.5, 3, 0.1);
			group_pause = new ButtonGroup(1, &button_pause);
		}

		// 게임 - 하단 메뉴
		{
			Button *button_toolbar[3] = { nullptr, };
			Callback cb[3] = {
				[=]() { OnEraseClicked(); },
				[=]() { OnMemoClicked(); },
				[=]() {}
			};
			CString string[3] = {
				CString("지우기"),
				CString("메모"),
				CString("힌트")
			};
			for (int i = 0; i < 3; i++) {
				Corner tl = [=](int width, int height) {
					return CPoint(
						width * 21 / 40 + height * 9 / 20 + height * (i * 3 - 4) / 18,
						height * 7 / 9
					);
					};
				Corner br = [=](int width, int height) {
					return CPoint(
						width * 21 / 40 + height * 9 / 20 + height * (i * 3 - 2) / 18,
						height * 8 / 9
					);
					};
				button_toolbar[i] = new EdgeButton(tl, br, cb[i], string[i], font_name, 0.3, 3, 0.1);
			}
			group_toolbar = new ButtonGroup(3, button_toolbar);
		}

		// 게임 - 격자판
		{
			Button *button_sudoku[81] = { nullptr, };
			for (int i = 0; i < 9 * 9; i++) {
				Corner tl = [=](int width, int height) {
					return CPoint(
						width / 20 + height * (i % 9) / 10,
						height * (2 * (i / 9) + 1) / 20
					);
					};
				Corner br = [=](int width, int height) {
					return CPoint(
						width / 20 + height * (i % 9 + 1) / 10,
						height * (2 * (i / 9) + 3) / 20
					);
					};
				button_sudoku[i] = new TextButton(tl, br, [=]() { OnSudokuMapClicked(i); }, CString(""), CString("굴림"), 0.7);
			}
			group_sudoku = new ButtonGroup(81, button_sudoku);
		}

		// 게임 - 완료
		{
			Corner tl = [](int width, int height) {
				return CPoint(
					width * 21 / 40 + height * 9 / 20 - height / 6,
					height * 7 / 9
				);
				};
			Corner br = [](int width, int height) {
				return CPoint(
					width * 21 / 40 + height * 9 / 20 + height / 6,
					height * 7 / 9 + height / 10
				);
				};
			Corner sp = [](int width, int height) {
				return CPoint(width * 3 / 2, height * 7 / 9);
				};
			Button *button_done = new AnimationButton(tl, br, [=]() { OnDoneClicked(); }, CString("완료"), font_name, 0.5, sp, 0.4, 0b11);
			group_done = new ButtonGroup(1, &button_done);
		}
	}

	return 0;
}


void CsudokuView::OnNewgameClicked() {
	group_newgame->Enable();
	m_menu = NEW_GAME;
}

void CsudokuView::OnDifficultyClicked(DIFF diff) {
	m_diff = diff;
	m_bMemo = false;
	m_clockRequested = clock();

	int blank = 0;
	switch (diff) {
	case EASY:
		blank = 20;
		break;
	case MEDIUM:
		blank = 40;
		break;
	case HARD:
		blank = 100;
		break;
	}
	
	std::thread([&]() {
		m_mutex.lock();
		m_map = new SudokuMap(blank);
		m_mutex.unlock();
	}).detach();
	while (m_mutex.try_lock()) m_mutex.unlock();
	m_mode = LOADING;
}

void CsudokuView::OnContinueClicked() {
	CFileDialog dlg(TRUE, NULL, NULL, 0, _T("Sudoku Save File (*.sdk)|*.sdk|"), this);
	m_menu = CONTINUE;
	if (dlg.DoModal() == IDOK) {
		
	}
	else {
		group_init->Enable();
		m_menu = START;
	}
}

void CsudokuView::OnSettingsClicked() {
	
	group_settings->Enable();
	m_menu = SETTINGS;
}

void CsudokuView::OnUserClicked() {

}

void CsudokuView::OnBackNewGameClicked() {
	group_init->Enable();
	m_menu = START;
}

void CsudokuView::OnSoundVolumeClicked(int inc)
{
	m_nSoundVolume = max(min(m_nSoundVolume + inc * 10, 100), 0);

	CString a;
	a.Format(_T("%d%%"), m_nSoundVolume);
	((TextButton *)(group_settings->group[4]))->ChangeText(a);

	COLORREF l = RGB(0, 0, 0), r = RGB(0, 0, 0);
	if (m_nSoundVolume == 0)
		l = RGB(200, 200, 200);
	if (m_nSoundVolume == 100)
		r = RGB(200, 200, 200);
	((TextButton *)(group_settings->group[7]))->ChangeTextColor(l);
	((TextButton *)(group_settings->group[8]))->ChangeTextColor(r);
}

void CsudokuView::OnScreenSizeClicked(int inc)
{
	m_nScreenRatio = max(min(m_nScreenRatio + inc, SCREEN_RATIO_COUNT - 1), 0);

	CRect fullrect, clientrect;
	AfxGetMainWnd()->GetWindowRect(&fullrect);
	GetWindowRect(&clientrect);
	AfxGetMainWnd()->MoveWindow(CRect(
		fullrect.TopLeft().x,
		fullrect.TopLeft().y,
		fullrect.TopLeft().x + SCREEN_RATIO[m_nScreenRatio][0] + fullrect.Width() - clientrect.Width(),
		fullrect.TopLeft().y + SCREEN_RATIO[m_nScreenRatio][1] + fullrect.Height() - clientrect.Height()
	));

	CString a;
	a.Format(_T("%d×%d"), SCREEN_RATIO[m_nScreenRatio][0], SCREEN_RATIO[m_nScreenRatio][1]);
	((TextButton *)(group_settings->group[5]))->ChangeText(a);

	COLORREF l = RGB(0, 0, 0), r = RGB(0, 0, 0);
	if (m_nScreenRatio == 0)
		l = RGB(200, 200, 200);
	if (m_nScreenRatio == SCREEN_RATIO_COUNT - 1)
		r = RGB(200, 200, 200);
	((TextButton *)(group_settings->group[9]))->ChangeTextColor(l);
	((TextButton *)(group_settings->group[10]))->ChangeTextColor(r);
}

void CsudokuView::OnLanguageClicked(int inc)
{
	m_nLanguage = (m_nLanguage + LANG_COUNT + inc) % LANG_COUNT;
}

void CsudokuView::OnBackSettingsClicked() {
	CString sound, screen, lang;
	sound.Format(_T("sound: %d\n"), m_nSoundVolume);
	screen.Format(_T("screen: %d\n"), m_nScreenRatio);
	lang.Format(_T("lang: %d\n"), m_nLanguage);
	CString st = sound + screen + lang;
	char buffer[1024] = { 0, };
	for (int i = 0; i < st.GetLength(); i++)
		buffer[i] = (char)st[i];

	CStdioFile preset;
	preset.Open(_T("settings.dat"), CFile::modeWrite | CFile::typeText);
	preset.Write(buffer, sizeof(char) * st.GetLength());
	preset.Close();

	group_init->Enable();
	m_menu = START;
}

void CsudokuView::OnNumberKeyClicked(int num) {
	if (m_bMemo)
		m_map->ToggleMemo(num, m_nSelRow, m_nSelCol);
	else
		m_map->SetValue(num, m_nSelRow, m_nSelCol);
}

void CsudokuView::OnEraseClicked()
{
	m_map->SetValue(0, m_nSelRow, m_nSelCol);
}

void CsudokuView::OnMemoClicked()
{
	m_bMemo = !m_bMemo;
	if (m_bMemo)
		((TextButton *)(group_toolbar->group[1]))->ChangeTextColor(RGB(255, 0, 0));
	else
		((TextButton *)(group_toolbar->group[1]))->ChangeTextColor(RGB(0, 0, 0));
}


void CsudokuView::OnSudokuMapClicked(int i) {
	m_nSelRow = i / 9;
	m_nSelCol = i % 9;
}

void CsudokuView::OnExitClicked() {
	AfxGetApp()->m_pMainWnd->PostMessage(WM_CLOSE);
}

void CsudokuView::OnDoneClicked() {
	group_sudoku->Disable();
	group_init->Enable();
	m_mode = INIT;
	m_menu = START;
	delete m_map;
	m_map = nullptr;
}


void CsudokuView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	KillTimer(0);

	char font_path[256] = { 0, };
	if (_getcwd(font_path, sizeof(font_path)) != nullptr) {
		strcat_s(font_path, "\\res\\MaruBuri-Regular.ttf");
		RemoveFontResource(CString(font_path));
	}
}


void CsudokuView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Button::Timer(nIDEvent);

	if (nIDEvent == 0) {

		if (m_mode == LOADING) {
			if (m_mutex.try_lock()) {
				m_mutex.unlock();
				m_nSelRow = m_nSelCol = 4;
				m_clockGenerated = clock();
				m_ingame = READY;
				m_mode = GAME;
				group_numberkey->Enable();
				group_pause->Enable();
				group_toolbar->Enable();
			}
		}
		else if (m_mode == GAME) {
			if (m_ingame == READY) {
				// 준비시간
				if (clock() - m_clockGenerated >= 3 * CLOCKS_PER_SEC) {
					m_ingame = ON;
					m_clockStarted = clock();
					group_sudoku->Enable();
				}
			}
			if (m_ingame == ON) {
				// Button과 SudokuMap 간의 데이터 교환
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
						if (m_map->GetValue(i, j) > 0) {
							CString a;
							a.Format(_T("%d"), m_map->GetValue(i, j));
							((TextButton *)group_sudoku->group[(size_t)i * 9 + j])->ChangeText(a);
							COLORREF color;
							if (m_map->Contradict(i, j))
								color = RGB(255, 0, 0);
							else if (m_map->Editable(i, j))
								color = RGB(100, 100, 255);
							else
								color = RGB(50, 50, 50);
							((TextButton *)group_sudoku->group[(size_t)i * 9 + j])->ChangeTextColor(color);
						}
						else
							((TextButton *)group_sudoku->group[(size_t)i * 9 + j])->ChangeText(CString(""));
				// 스도쿠 완료
				if (m_map->Done()) {
					m_clockEnded = clock();
					group_numberkey->Disable();
					group_pause->Disable();
					group_toolbar->Disable();

					ranking[m_diff]->push_back((double)(m_clockEnded - m_clockStarted) / CLOCKS_PER_SEC);
					std::sort(ranking[m_diff]->begin(), ranking[m_diff]->end());
					while (ranking[m_diff]->size() > 5)
						ranking[m_diff]->pop_back();

					CString st;
					CString diff[3] = {CString("easy"), CString("medium"), CString("hard")};
					for (int d = 0; d < 3; d++) {
						for (int i = 0; i < ranking[d]->size(); i++) {
							CString a, b;
							a.Format(_T("%d"), i + 1);
							b.Format(_T("%.2f\n"), ranking[d]->at(i));
							st += diff[d] + a + CString(": ") + b;
						}
						for (int i = (int)ranking[d]->size(); i < 5; i++) {
							CString a;
							a.Format(_T("%d"), i + 1);
							st += diff[d] + a + CString(": ") + CString("-1\n");
						}
					}

					char buffer[1024] = { 0, };
					for (int i = 0; i < st.GetLength(); i++)
						buffer[i] = (char)st[i];

					CStdioFile rank;
					rank.Open(_T("ranking.dat"), CFile::modeWrite | CFile::typeText);
					rank.Write(buffer, sizeof(char) * st.GetLength());
					rank.Close();

					m_ingame = DONE;
					group_done->Enable();
				}
			}
		}

		Invalidate(TRUE);
	}

	CView::OnTimer(nIDEvent);
}

void CsudokuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Button::Mouse(nFlags, point);

	CView::OnMouseMove(nFlags, point);
}


void CsudokuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Button::Click(nFlags, point);

	CView::OnLButtonDown(nFlags, point);
}


void CsudokuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Button::Keyboard(nChar, nRepCnt, nFlags);

	if (m_mode == GAME) {
		if (m_ingame == ON) {
			if ('1' <= nChar && nChar <= '9')
				OnNumberKeyClicked(nChar - '0');
			else if (nChar == VK_DELETE || nChar == VK_BACK || nChar == '0')
				OnEraseClicked();
		}
		if (m_ingame == ON || m_ingame == DONE) {
			if (nChar == VK_UP)
				m_nSelRow = max(0, m_nSelRow - 1);
			else if (nChar == VK_DOWN)
				m_nSelRow = min(8, m_nSelRow + 1);
			else if (nChar == VK_LEFT)
				m_nSelCol = max(0, m_nSelCol - 1);
			else if (nChar == VK_RIGHT)
				m_nSelCol = min(8, m_nSelCol + 1);
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CsudokuView::OnEraseBkgnd(CDC *pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
}


void CsudokuView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	Button::Size(nType, cx, cy);
}
