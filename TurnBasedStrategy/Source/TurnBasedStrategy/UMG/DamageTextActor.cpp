// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextActor.h"
#include "Components/WidgetComponent.h"
#include "DamageTextWidget.h"

// Sets default values
ADamageTextActor::ADamageTextActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	SetRootComponent(WidgetComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	InitialLifeSpan = 1.5f;
	Speed = 100.0f;
}

// Called when the game starts or when spawned
void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (DamageTextClass)
	{
		WidgetComponent->SetWidgetClass(DamageTextClass);
		UDamageTextWidget* damageWidget = Cast<UDamageTextWidget>(WidgetComponent->GetWidget());
		if (damageWidget)
		{
			damageWidget->SetDamageText(AppliedDamage);
			damageWidget->SetColorAndOpacity(FLinearColor(0.85f, 0.1f, 0.1f, 1.0f));
		}
	}

}

// Called every frame
void ADamageTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + FVector::UpVector * DeltaTime * Speed);
}

