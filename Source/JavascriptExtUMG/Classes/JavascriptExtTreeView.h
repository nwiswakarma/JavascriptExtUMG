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

#pragma once

#include "STreeView.h"
#include "TableViewBase.h"
#include "JavascriptExtTreeView.generated.h"

class UJavascriptContext;

USTRUCT(BlueprintType)
struct FJavascriptExtColumn
{
    GENERATED_BODY()
    
    UPROPERTY()
    FName Id;
    
    UPROPERTY()
    float Width;
    
    UPROPERTY(Transient)
    UWidget* Widget;
};

/**
* Allows thousands of items to be displayed in a list.  Generates widgets dynamically for each item.
*/
UCLASS(Experimental)
class JAVASCRIPTEXTUMG_API UJavascriptExtTreeView : public UTableViewBase
{
	GENERATED_UCLASS_BODY()

public:	
	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetChildrenUObject, UObject*, Item, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnExpansionChanged, UObject*, Item, bool, bExpanded, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(UWidget*, FOnGenerateRow, UObject*, Object, FName, Id, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(UWidget*, FOnContextMenuOpening, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnColumnRefreshed, FName, Id, float, NewSize, UJavascriptExtTreeView*, Instance);

	/** Called when a widget needs to be generated */
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnGenerateRow OnGenerateRowEvent;
	
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnExpansionChanged OnExpansionChanged;

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnContextMenuOpening OnContextMenuOpening;

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnColumnRefreshed OnColumnRefreshed;

	/** Called when a widget needs to be generated */
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnGetChildrenUObject OnGetChildren;	

	UPROPERTY(BlueprintReadWrite, Category = "Javascript")
	UJavascriptContext* JavascriptContext;

	/** The list of items to generate widgets for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content)
	TArray<UObject*> Items;

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly, Category = "Javascript")
	FHeaderRowStyle HeaderRowStyle;

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly, Category = "Javascript")
	FTableRowStyle TableRowStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Javascript")
	FScrollBarStyle ScrollBarStyle;

	/** The selection method for the list */
	UPROPERTY(EditAnywhere, Category = Content)
	TEnumAsByte<ESelectionMode::Type> SelectionMode;

	UPROPERTY(BlueprintReadWrite, Transient, Category = "Javascript")
	mutable TArray<UObject*> Children;

	UPROPERTY(BlueprintReadWrite, Transient, Category = "Javascript")
	TArray<FJavascriptExtColumn> Columns;

	/** Refreshes the list */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void RequestTreeRefresh();

	/** Event fired when a tutorial stage ends */
	UFUNCTION(BlueprintImplementableEvent, Category = "Javascript")
	void OnDoubleClick(UObject* Object);

	/** Event fired when a tutorial stage ends */
	UFUNCTION(BlueprintImplementableEvent, Category = "Javascript")
	void OnSelectionChanged(UObject* Object, ESelectInfo::Type Type);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void GetSelectedItems(TArray<UObject*>& OutItems);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetSelection(UObject* SoleSelectedItem);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetItemExpansion(UObject* InItem, bool InShouldExpandItem);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetSingleExpandedItem(UObject* InItem);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetColumnSize(FName ColumnId, float InSize);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	bool IsItemExpanded(UObject* InItem);

	TSharedRef<ITableRow> HandleOnGenerateRow(UObject* Item, const TSharedRef< STableViewBase >& OwnerTable);

	void HandleOnGetChildren(UObject* Item, TArray<UObject*>& OutChildItems);
	void HandleOnExpansionChanged(UObject* Item, bool bExpanded);
	void HandleOnColumnRefreshed();

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	// End of UWidget interface

	// UObject interface
	virtual void ProcessEvent(UFunction* Function, void* Parms) override;
	// End of UObject interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	TSharedPtr< STreeView<UObject*> > MyTreeView;

	TSharedPtr<SHeaderRow> GetHeaderRowWidget();

	UPROPERTY(Transient)
	TArray<UWidget*> ColumnWidgets;

	// UObject interface.
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	TMultiMap<UObject*, TWeakPtr<SWidget>> CachedRows;

protected:

	TWeakPtr<SHeaderRow> HeaderRow;

};
