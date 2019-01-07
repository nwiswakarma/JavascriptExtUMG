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

#include "JavascriptExtTreeView.h"
#include "JavascriptContext.h"
#include "SlateOptMacros.h"

UJavascriptExtTreeView::UJavascriptExtTreeView(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{	
	bIsVariable = true;

	SelectionMode = ESelectionMode::Single;

	HeaderRowStyle = FCoreStyle::Get().GetWidgetStyle<FHeaderRowStyle>("TableView.Header");
	TableRowStyle = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	ScrollBarStyle = FCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar");
}

TSharedPtr<SHeaderRow> UJavascriptExtTreeView::GetHeaderRowWidget()
{
	TSharedRef<SHeaderRow> HeaderRowWidget = SNew(SHeaderRow).Style(&HeaderRowStyle);

	if (Columns.Num())
	{
		ColumnWidgets.Empty();

		for (auto& Column : Columns)
		{
            FName& ColumnId(Column.Id);

			if (!Column.Widget)
			{
				if (OnGenerateRowEvent.IsBound())
				{
					Column.Widget = OnGenerateRowEvent.Execute(nullptr, Column.Id, this);
					ColumnWidgets.Add(Column.Widget);
				}
			}

			if (!Column.Widget)
			{
				continue;
			}

			HeaderRowWidget->AddColumn(
				SHeaderRow::Column(Column.Id)
				.FillWidth(Column.Width)
				[
					Column.Widget->TakeWidget()
				]
			);
		}
	}
	return HeaderRowWidget;
}

TSharedRef<SWidget> UJavascriptExtTreeView::RebuildWidget()
{
    TSharedPtr<SHeaderRow> NewHeaderRow = GetHeaderRowWidget();
	TSharedRef<SScrollBar> ExternalScrollbar = SNew(SScrollBar).Style(&ScrollBarStyle);
	TSharedRef<SWidget> MyView = StaticCastSharedRef<SWidget>
	(
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SAssignNew(MyTreeView, STreeView< UObject* >)
			.SelectionMode(SelectionMode)
			.TreeItemsSource(&Items)
			.OnGenerateRow(BIND_UOBJECT_DELEGATE(STreeView< UObject* >::FOnGenerateRow, HandleOnGenerateRow))
			.OnGetChildren(BIND_UOBJECT_DELEGATE(STreeView< UObject* >::FOnGetChildren, HandleOnGetChildren))
			.OnExpansionChanged(BIND_UOBJECT_DELEGATE(STreeView< UObject* >::FOnExpansionChanged, HandleOnExpansionChanged))
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
            .OnSelectionChanged_Lambda([this](UObject* Object, ESelectInfo::Type SelectInfo) {
                OnSelectionChanged(Object, SelectInfo);
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

void UJavascriptExtTreeView::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);
    HandleOnColumnRefreshed();
}

void UJavascriptExtTreeView::ProcessEvent(UFunction* Function, void* Parms)
{
	if (JavascriptContext && JavascriptContext->CallProxyFunction(this, this, Function, Parms))
	{
		return;
	}

	Super::ProcessEvent(Function, Parms);
}

void UJavascriptExtTreeView::RequestTreeRefresh()
{
	if (MyTreeView.IsValid())
	{
        HandleOnColumnRefreshed();
		MyTreeView->RequestTreeRefresh();
	}	
}

/**
* Implements a row widget for the session console log.
*/
class SJavascriptTableRow
	: public SMultiColumnTableRow<UObject*>, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SJavascriptTableRow) { }
		SLATE_ARGUMENT(UObject*, Object)
		SLATE_ARGUMENT(UJavascriptExtTreeView*, TreeView)
		SLATE_STYLE_ARGUMENT(FTableRowStyle, Style)
	SLATE_END_ARGS()

public:
	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObjects(Widgets);
		Collector.AddReferencedObject(Object);
	}
	// End of FSerializableObject interface

	/**
	* Constructs the widget.
	*
	* @param InArgs The construction arguments.
	* @param InOwnerTableView The table view that owns this row.
	*/
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Object = InArgs._Object;
		TreeView = InArgs._TreeView;

		SMultiColumnTableRow<UObject*>::Construct(FSuperRowType::FArguments().Style(InArgs._Style), InOwnerTableView);
	}

public:

	// SMultiColumnTableRow interface

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		auto ColumnWidget = SNullWidget::NullWidget;

		if (TreeView->OnGenerateRowEvent.IsBound())
		{
			UWidget* Widget = TreeView->OnGenerateRowEvent.Execute(Object, ColumnName, TreeView);

			if (Widget)
			{
				ColumnWidget = Widget->TakeWidget();
				Widgets.Add(Widget);
			}
		}

		if (TreeView->IsA(UJavascriptExtTreeView::StaticClass()) && ColumnName == TreeView->Columns[0].Id)
		{
			// The first column gets the tree expansion arrow for this row
			return
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					ColumnWidget
				];
		}
		else
		{
			return ColumnWidget;
		}
	}
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	TArray<UWidget*> Widgets;

private:
	UObject* Object;
	UJavascriptExtTreeView* TreeView;
};

TSharedRef<ITableRow> UJavascriptExtTreeView::HandleOnGenerateRow(UObject* Item, const TSharedRef< STableViewBase >& OwnerTable)
{
	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if (OnGenerateRowEvent.IsBound())
	{
		if (Columns.Num())
		{
			return SNew(SJavascriptTableRow, OwnerTable).Object(Item).TreeView(this).Style(&TableRowStyle);
		}
		else
		{
			UWidget* Widget = OnGenerateRowEvent.Execute(Item, FName(), this);
			if (Widget != NULL)
			{
				auto GeneratedWidget = Widget->TakeWidget();
				CachedRows.Add(Widget, GeneratedWidget);
				return SNew(STableRow< UObject* >, OwnerTable)[GeneratedWidget];
			}
		}		
	}

	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STableRow< UObject* >, OwnerTable)
		[
			SNew(STextBlock).Text(Item ? FText::FromString(Item->GetName()) : FText::FromName(FName()))
		];
}


void UJavascriptExtTreeView::HandleOnGetChildren(UObject* Item, TArray<UObject*>& OutChildItems)
{
	if (OnGetChildren.IsBound())
	{
		Children.Empty();

		OnGetChildren.Execute(Item,this);
		
		OutChildItems.Append(Children);

		Children.Empty();
	}
}

void UJavascriptExtTreeView::HandleOnExpansionChanged(UObject* Item, bool bExpanded)
{
	if (OnExpansionChanged.IsBound())
	{
		OnExpansionChanged.Execute(Item, bExpanded, this);
	}
}

void UJavascriptExtTreeView::HandleOnColumnRefreshed()
{
	if (OnColumnRefreshed.IsBound())
	{
        if (HeaderRow.IsValid())
        {
            TSharedPtr<SHeaderRow> pHeaderRow = HeaderRow.Pin();
            for (auto& col : pHeaderRow->GetColumns())
                OnColumnRefreshed.Execute(col.ColumnId, col.GetWidth(), this);
        }
	}
}

void UJavascriptExtTreeView::GetSelectedItems(TArray<UObject*>& OutItems)
{
	if (MyTreeView.IsValid())
	{
		OutItems = MyTreeView->GetSelectedItems();
	}
}

void UJavascriptExtTreeView::SetItemExpansion(UObject* InItem, bool InShouldExpandItem)
{
	if (MyTreeView.IsValid())
	{
		MyTreeView->SetItemExpansion(InItem, InShouldExpandItem);
	}
}

void UJavascriptExtTreeView::SetSingleExpandedItem(UObject* InItem)
{
	if (MyTreeView.IsValid())
	{
		MyTreeView->SetSingleExpandedItem(InItem);
	}
}

void UJavascriptExtTreeView::SetSelection(UObject* SoleSelectedItem)
{
	if (MyTreeView.IsValid())
	{
		MyTreeView->SetSelection(SoleSelectedItem);
	}
}

void UJavascriptExtTreeView::SetColumnSize(FName ColumnId, float InSize)
{
    for (auto& col : Columns)
    {
        if (col.Id == ColumnId)
        {
            col.Width = InSize;
            break;
        }
    }
}

bool UJavascriptExtTreeView::IsItemExpanded(UObject* InItem)
{
	return MyTreeView.IsValid() && MyTreeView->IsItemExpanded(InItem);
}

void UJavascriptExtTreeView::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	auto This = static_cast<UJavascriptExtTreeView*>(InThis);

	if (This->MyTreeView.IsValid())
	{
		for (auto It = This->CachedRows.CreateIterator(); It; ++It)
		{
			auto Key = It->Key;
			auto Value = It->Value;

			if (Value.IsValid())
			{
				Collector.AddReferencedObject(Key, This);
			}
			else
			{
				It.RemoveCurrent();
			}
		}
	}
	else
	{
		This->CachedRows.Empty();
	}

	Super::AddReferencedObjects(This, Collector);
}

void UJavascriptExtTreeView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyTreeView.Reset();
}
