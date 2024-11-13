/*
Copyright (c) 2024 University Libraries, University of Nevada, Reno

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/ 

#include "SimpleCaptionComponent.h"
#include "GameTime.h"

// Sets default values for this component's properties
USimpleCaptionComponent::USimpleCaptionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USimpleCaptionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USimpleCaptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(Overlays)) return;
	if (!Playing) return;
	if (CurrentOverlayIndex >= Overlays->GetAllOverlays().Num())
	{
		Playing = false;
		return;
	}

	double now = FGameTime::GetTimeSinceAppStart().GetWorldTimeSeconds();
	if (now >= CurrentOverlayEndTime + PlaybackStartTime)
	{
		++CurrentOverlayIndex;
		
		if (CurrentOverlayIndex >= Overlays->GetAllOverlays().Num())
		{
			Playing = false;
			return;
		}
		else
		{
			double ms = Overlays->GetAllOverlays()[CurrentOverlayIndex].EndTime.GetTotalMilliseconds() / 1000.0;
			CurrentOverlayEndTime = ms;
		}
	}
}

void USimpleCaptionComponent::StartPlayback()
{
	if (!IsValid(Overlays)) return;
	if (Overlays->GetAllOverlays().Num() < 1) return;

	PlaybackStartTime = FGameTime::GetTimeSinceAppStart().GetWorldTimeSeconds();

	Playing = true;
	CurrentOverlayIndex = 0;
	CurrentOverlayEndTime = Overlays->GetAllOverlays()[CurrentOverlayIndex].EndTime.GetTotalMilliseconds() / 1000.0;
}

void USimpleCaptionComponent::StopPlayback()
{
	Playing = false;
}

void USimpleCaptionComponent::SetOverlays(UBasicOverlays* NewOverlay)
{
	StopPlayback();
	Overlays = NewOverlay;
	if (IsValid(Overlays))
	{
		NumOverlays = Overlays->GetAllOverlays().Num();
	}
}

void USimpleCaptionComponent::GetCurrentText(FString& OutputText) const
{
	if (!IsValid(Overlays) || !Playing)
	{
		OutputText = "";
		return;
	}

	const auto OverlayArray = Overlays->GetAllOverlays();
	if (CurrentOverlayIndex <= OverlayArray.Num())
	{
		OutputText = OverlayArray[CurrentOverlayIndex].Text;
	}
}
