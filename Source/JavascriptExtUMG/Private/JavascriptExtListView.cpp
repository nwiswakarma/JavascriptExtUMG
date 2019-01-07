// MIT License
// 
// Copyright (c) 2018-2019 Nuraga Wiswakarma
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "JavascriptExtListView.h"
#include "JavascriptContext.h"

UJavascriptExtListView::UJavascriptExtListView(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{	
}

TSharedRef<SWidget> UJavascriptExtListView::RebuildWidget()
{
    TSharedPtr<SHeaderRow> NewHeaderRow = GetHeaderRowWidget();
	TSharedRef<SScrollBar> ExternalScrollbar = SNew(SScrollBar).Style(&ScrollBarStyle);
	TSharedRef<SWidget> MyView = StaticCastSharedRef<SWidget>
	(
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SAssignNew(MyListView, SListView< UObject* >)
			.SelectionMode(SelectionMode)
			.ListItemsSource(&Items)
			.ItemHeight(ItemHeight)
			.OnContextMenuOpening_Lambda([this]() {
				if (OnContextMenuOpening.IsBound())
				{
					auto Widget = OnContextMenuOpening.Execute(this);
					if (Widget)
					{
						return Widget->TakeWidget();
					}
				}
				return SNullWidget::NullWidget;
			})
			.OnGenerateRow(BIND_UOBJECT_DELEGATE(SListView< UObject* >::FOnGenerateRow, HandleOnGenerateRow))
			.OnSelectionChanged_Lambda([this](UObject* Object, ESelectInfo::Type SelectInfo) {
                OnSelectionChanged(Object, SelectInfo);
			})
			.OnMouseButtonClick_Lambda([this](UObject* Object) {
				OnClick(Object);
			})
			.OnMouseButtonDoubleClick_Lambda([this](UObject* Object) {
				OnDoubleClick(Object);
			})
			.HeaderRow(NewHeaderRow)
			.ExternalScrollbar(ExternalScrollbar)
			//.OnContextMenuOpening(this, &SSocketManager::OnContextMenuOpening)
			//.OnItemScrolledIntoView(this, &SSocketManager::OnItemScrolledIntoView)
			//	.HeaderRow
			//	(
			//		SNew(SHeaderRow)
			//		.Visibility(EVisibility::Collapsed)
			//		+ SHeaderRow::Column(TEXT("Socket"))
			//	);
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(FOptionalSize(16))
			[
				ExternalScrollbar
			]
		]
	);
    HeaderRow = NewHeaderRow;
    HandleOnColumnRefreshed();
	return MyView;
}

void UJavascriptExtListView::RequestListRefresh()
{
	if (MyListView.IsValid())
	{
        HandleOnColumnRefreshed();
		MyListView->RequestListRefresh();
	}	
}

void UJavascriptExtListView::GetSelectedItems(TArray<UObject*>& OutItems)
{
	if (MyListView.IsValid())
	{
		OutItems = MyListView->GetSelectedItems();
	}
}

void UJavascriptExtListView::SetSelection(UObject* SoleSelectedItem)
{
	if (MyListView.IsValid())
	{
		MyListView->SetSelection(SoleSelectedItem);
	}
}
