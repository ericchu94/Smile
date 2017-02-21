// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SmileDetector.h"

#include "Components/ActorComponent.h"
#include "SmileComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMILE_API USmileComponent : public UActorComponent
{
	GENERATED_BODY()

	SmileDetector *Detector;
	FRunnableThread* Thread;

public:	
	// Sets default values for this component's properties
	USmileComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game starts
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	int CameraIndex = 0;

	UPROPERTY(EditAnywhere)
	EFaceRecognizerEnum FaceRecognizerEnum = EFaceRecognizerEnum::FRE_FisherFace;

	UFUNCTION(BlueprintCallable, Category = "Smile")
	void AddSample(bool positive);

	UFUNCTION(BlueprintCallable, Category = "Smile")
	void ResetSamples();

	UFUNCTION(BlueprintCallable, Category = "Smile")
	void StartPredicting();

	UFUNCTION(BlueprintCallable, Category = "Smile")
	void StopPredicting();

	UFUNCTION(BlueprintCallable, Category = "Smile")
	bool IsSmiling();

	UFUNCTION(BlueprintCallable, Category = "Smile")
	UTexture2D* GetTexture();

	UFUNCTION(BlueprintCallable, Category = "Smile")
	void UpdateTexture();
};
