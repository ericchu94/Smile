// Fill out your copyright notice in the Description page of Project Settings.

#include "Smile.h"
#include "SmileDetector.h"

SmileDetector::SmileDetector(int CameraIndex, EFaceRecognizerEnum FaceRecognizerEnum) : CameraIndex(CameraIndex), FaceRecognizerEnum(FaceRecognizerEnum)
{
	switch (FaceRecognizerEnum) {
	case EFaceRecognizerEnum::FRE_EigenFace:
		Model = cv::face::createEigenFaceRecognizer();
		break;
	case EFaceRecognizerEnum::FRE_FisherFace:
		Model = cv::face::createFisherFaceRecognizer();
		break;
	case EFaceRecognizerEnum::FRE_LBPHFace:
		Model = cv::face::createLBPHFaceRecognizer();
		break;
	}

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFaceRecognizerEnum"), true);
	UE_LOG(LogTemp, Log, TEXT("Using %s"), *EnumPtr->GetEnumName((int32)FaceRecognizerEnum));
}

SmileDetector::~SmileDetector()
{
}

bool SmileDetector::Init() {
	if (!Capture.open(CameraIndex)) {
		UE_LOG(LogTemp, Error, TEXT("Unable to open camera %d"), CameraIndex);
		return false;
	}

	cv::Mat Raw;
	Capture >> Raw;

	// Calculate square region
	int Side = std::min(Raw.rows, Raw.cols);
	int DX = (Raw.cols - Side) / 2;
	int DY = (Raw.rows - Side) / 2;
	Region = cv::Rect(DX, DY, Side, Side);

	Texture = UTexture2D::CreateTransient(Side, Side, PF_B8G8R8A8);

	CaptureFrame();

	return true;
}

uint32 SmileDetector::Run()
{
	while (!ShouldStop)
	{
		cv::Mat Frame = CaptureFrame();

		if (AddPositive) {
			AddPositive = false;
			TrainingImages.push_back(Frame.clone());
			TrainingLabels.push_back(1);
		}
		if (AddNegative) {
			AddNegative = false;
			TrainingImages.push_back(Frame.clone());
			TrainingLabels.push_back(0);
		}

		if (Reset) {
			Reset = false;
			TrainingImages.clear();
			TrainingLabels.clear();
		}

		if (Train) {
			Train = false;
			Model->train(TrainingImages, TrainingLabels);
		}

		if (Predict) {
			Smiling = Model->predict(Frame) == 1;
		}
	}
	return 0;
}

void SmileDetector::Stop()
{
	ShouldStop = true;
}

void SmileDetector::Exit()
{
	Capture.release();
}

cv::Mat SmileDetector::CaptureFrame() {
	// Read from camera
	cv::Mat Raw;
	Capture >> Raw;

	// Crop image to square
	CriticalSection.Lock();
	FrameSquare = Raw(Region);
	CriticalSection.Unlock();

	// Convert to grayscale
	cv::Mat Frame;
	cv::cvtColor(FrameSquare, Frame, CV_BGR2GRAY);

	return Frame;
}

void SmileDetector::AddSample(bool positive) {
	if (positive) {
		AddPositive = true;
	}
	else {
		AddNegative = true;
	}
}

void SmileDetector::ResetSamples() {
	Reset = true;
}

void SmileDetector::StartPredicting() {
	if (!Predict) {
		Train = true;
		Predict = true;
	}
}

void SmileDetector::StopPredicting() {
	Predict = false;
}

bool SmileDetector::IsSmiling() {
	return Smiling;
}

UTexture2D* SmileDetector::GetTexture() {
	return Texture;
}

void SmileDetector::UpdateTexture() {
	// Mirror
	cv::Mat Flipped;
	CriticalSection.Lock();
	cv::flip(FrameSquare, Flipped, 1);
	CriticalSection.Unlock();

	// Add alpha channel
	cv::Mat BGRA;
	cv::cvtColor(Flipped, BGRA, CV_BGR2BGRA);

	// Update Texture
	void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, BGRA.data, BGRA.cols * BGRA.rows * 4);
	Texture->PlatformData->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();
}