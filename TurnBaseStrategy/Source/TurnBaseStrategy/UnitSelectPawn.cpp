// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSelectPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h" //UKismetMathLibrary
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnitCharacter.h"

#include "UnitMoveActionComponent.h"
#include "GridManager.h"
#include "Grid.h"

// Sets default values
AUnitSelectPawn::AUnitSelectPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
}

// Called when the game starts or when spawned
void AUnitSelectPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitSelectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitSelectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUnitSelectPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnitSelectPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AUnitSelectPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AUnitSelectPawn::LookUp);

	PlayerInputComponent->BindAction("UnitSelect", IE_Pressed, this, &AUnitSelectPawn::UnitSelect);

}

void AUnitSelectPawn::MoveForward(float Value)
{
	FVector cameraLocation;
	FRotator cameraRotation;
	GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FRotator controlRot = cameraRotation;
	FVector temp = UKismetMathLibrary::GetForwardVector(controlRot);

	AddMovementInput(temp, Value);
}

void AUnitSelectPawn::MoveRight(float Value)
{
	FVector cameraLocation;
	FRotator cameraRotation;
	GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FRotator controlRot = cameraRotation;
	FVector temp = UKismetMathLibrary::GetRightVector(controlRot);

	AddMovementInput(temp, Value);
}

void AUnitSelectPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AUnitSelectPawn::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AUnitSelectPawn::UnitSelect()
{
	UE_LOG(LogTemp, Warning, TEXT("UnitSelect()"));

	APlayerController* playerController= UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (!IsValid(playerController))
	{
		return;
	}

	FVector loc;
	FVector rot;
	playerController->DeprojectMousePositionToWorld(loc, rot);

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;

	//objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	//objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	//objects.Add(EObjectTypeQuery::ObjectTypeQuery7);

	TArray<AActor*> ignores;
	ignores.Add(this);

	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit, loc, loc + rot * 1000, ECollisionChannel::ECC_Visibility);
	bool result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		loc,
		loc + rot * 10000,
		objects,
		true,
		ignores,
		//EDrawDebugTrace::None,
		EDrawDebugTrace::ForDuration,
		hit,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.0f
	);

	if (result)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unit Name : %s"), *hit.GetActor()->GetName());
		if (hit.GetActor()->IsA(AUnitCharacter::StaticClass()))
		{
			SelectedUnit = Cast<AUnitCharacter>(hit.GetActor());

			UUnitMoveActionComponent* moveActionComponent = SelectedUnit->UnitMoveActionComponent;
			if (IsValid(moveActionComponent))
			{
				auto gridarray= moveActionComponent->GetValidActionGridArray();
				AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
				if (IsValid(gridManager))
				{
					gridManager->HideAllGridVisual();
					gridManager->ShowFromGridArray(gridarray,EGridVisualType::Blue);
				}

			}

		}
	}

}

