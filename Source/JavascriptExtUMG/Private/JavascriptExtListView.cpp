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
