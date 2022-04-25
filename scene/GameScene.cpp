#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete sprite_;
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	model_ = Model::Create();

	worldTransform_.rotation_ = {0, 0, 0};

	worldTransform_.Initialize();

	viewProjection_.eye = {0, 20, -30};
	viewProjection_.target = {0, 0, 0};
	viewProjection_.up = {0.0f, 1.0f, 0.0f};

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	XMFLOAT3 move = {0, 0, 0};

	//回転
	const float kRotSpeed = 0.005f;

	if (input_->PushKey(DIK_U)) {
		worldTransform_.rotation_.y += kRotSpeed;
	} else if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}

	const float kCharSpeed = 0.05f;

	//移動
	if (input_->PushKey(DIK_UP)) {
		move = {
		  sin(worldTransform_.rotation_.y) * kCharSpeed, 0,
		  cos(worldTransform_.rotation_.y) * kCharSpeed};
		if (input_->PushKey(DIK_DOWN)) {
			move = {0, 0, 0};
		}
	} else if (input_->PushKey(DIK_DOWN)) {
		move = {
		  -sin(worldTransform_.rotation_.y) * kCharSpeed, 0,
		  -cos(worldTransform_.rotation_.y) * kCharSpeed};
	}

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	worldTransform_.UpdateMatrix();
	
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngle:(Degree:%f)", XMConvertToDegrees(viewProjection_.fovAngleY));

	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);

	debugText_->SetPos(50, 150);
	debugText_->Printf(
	  "Root:(%f,%f,%f)", worldTransform_.translation_.x,
	  worldTransform_.translation_.y, worldTransform_.translation_.z);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/*model_->Draw(worldTransform_[PartId::Root], viewProjection_, textureHandle_);*/
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
