// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#pragma push_macro("check")
#undef check
#pragma warning( push )
#pragma warning( disable: 4530 )
#include <typeinfo>
namespace std { typedef type_info type_info; }
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#pragma warning( pop )
#pragma pop_macro("check")


UENUM(BlueprintType)
enum class EFaceRecognizerEnum : uint8 {
	FRE_EigenFace  UMETA(DisplayName = "Eigen Face"),
	FRE_FisherFace UMETA(DisplayName = "Fisher Face"),
	FRE_LBPHFace   UMETA(DisplayName = "LBPH Face")
};

/**
 * 
 */
class SMILE_API SmileDetector : public FRunnable
{
	int CameraIndex;
	EFaceRecognizerEnum FaceRecognizerEnum;

	cv::Ptr<cv::face::FaceRecognizer> Model;
	std::vector<cv::Mat> TrainingImages;
	std::vector<int> TrainingLabels;

	cv::VideoCapture Capture;
	cv::Rect Region;
	cv::Mat FrameSquare;

	FCriticalSection CriticalSection;
	UTexture2D *Texture;

	bool ShouldStop = false;
	bool AddPositive = false;
	bool AddNegative = false;
	bool Reset = false;
	bool Train = false;
	bool Predict = false;
	bool Smiling = false;

	cv::Mat CaptureFrame();
	
public:
	SmileDetector(int CameraIndex, EFaceRecognizerEnum FaceRecognizerEnum);
	~SmileDetector();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

	void AddSample(bool positive);
	void ResetSamples();
	void StartPredicting();
	void StopPredicting();
	bool IsSmiling();
	UTexture2D* GetTexture();
	void UpdateTexture();
};
