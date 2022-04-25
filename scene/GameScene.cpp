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


	worldTransform_[0].rotation_ = {0, 0, 0};

	worldTransform_[1].translation_ = {-15, 0, 15};
	worldTransform_[2].translation_ = {15, 0, 15};
	worldTransform_[3].translation_ = {-15, 0, -15};
	worldTransform_[4].translation_ = {15, 0, -15};

	//ワールドトランスフォームの初期化
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		worldTransform_[i].Initialize();
	}
	//ビュープロジェクションの初期座標
	viewProjection_.eye = {0, viewLengthY, viewLengthZ};
	viewProjection_.target = {0, 0, 0};
	viewProjection_.up = {0.0f, 1.0f, 0.0f};

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	//移動用座標
	XMFLOAT3 move = {0, 0, 0};

	//回転速度
	const float kRotSpeed = 0.005f;

	//左右回転処理
	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_[0].rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_[0].rotation_.y += kRotSpeed;
	}

	//移動速度
	const float kCharSpeed = 0.05f;

	//移動処理
	if (input_->PushKey(DIK_UP)) {

		move = {
		  sin(worldTransform_[0].rotation_.y) * kCharSpeed, 0,
		  cos(worldTransform_[0].rotation_.y) * kCharSpeed
		};

		if (input_->PushKey(DIK_DOWN)) {
			move = {0, 0, 0};
		}
	} else if (input_->PushKey(DIK_DOWN)) {

		move = {
		  -sin(worldTransform_[0].rotation_.y) * kCharSpeed, 0,
		  -cos(worldTransform_[0].rotation_.y) * kCharSpeed
		};

	}
	//移動を反映
	worldTransform_[0].translation_.x += move.x;
	worldTransform_[0].translation_.y += move.y;
	worldTransform_[0].translation_.z += move.z;

	///---カメラ追従---///
	//オブジェクトを追従
	viewProjection_.target = {
	  worldTransform_[0].translation_.x, worldTransform_[0].translation_.y,
	  worldTransform_[0].translation_.z
	};

	//カメラ位置を更新
	viewProjection_.eye.x =
	  worldTransform_[0].translation_.x -
	                        sin(worldTransform_[0].rotation_.y) *
	                          sqrt(viewLengthY * viewLengthY + viewLengthZ * viewLengthZ);
	//viewProjection_.eye.y = move.y;
	viewProjection_.eye.z =
	  worldTransform_[0].translation_.z -
	                        cos(worldTransform_[0].rotation_.y) *
	                          sqrt(viewLengthY * viewLengthY + viewLengthZ * viewLengthZ);

	//オブジェクト座標を更新
	worldTransform_[0].UpdateMatrix();
	//カメラ座標を更新
	viewProjection_.UpdateMatrix();
	
	//デバックテキストを表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

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
	  "translation:(%f,%f,%f)", worldTransform_[0].translation_.x, worldTransform_[0].translation_.y, worldTransform_[0].translation_.z);
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
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

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
