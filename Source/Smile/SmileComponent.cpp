// Fill out your copyright notice in the Description page of Project Settings.

#include "Smile.h"
#include "SmileComponent.h"
#include "SmileDetector.h"


// Sets default values for this component's properties
USmileComponent::USmileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void USmileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Detector = new SmileDetector(CameraIndex, FaceRecognizerEnum);
	Thread = FRunnableThread::Create(Detector, TEXT("SmileDetector"));
}

void USmileComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	Thread->Kill();
	delete Thread;
	Thread = NULL;
	delete Detector;
	Detector = NULL;
}

void USmileComponent::AddSample(bool positive) {
	Detector->AddSample(positive);
}

void USmileComponent::ResetSamples() {
	Detector->ResetSamples();
}

void USmileComponent::StartPredicting() {
	Detector->StartPredicting();
}

void USmileComponent::StopPredicting() {
	Detector->StopPredicting();
}

bool USmileComponent::IsSmiling() {
	return Detector->IsSmiling();
}

UTexture2D* USmileComponent::GetTexture() {
	return Detector->GetTexture();
}

void USmileComponent::UpdateTexture() {
	Detector->UpdateTexture();
}