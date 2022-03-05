<template>
	<view class="content">
		<view class="bar head">
			<text>
				头像
			</text>
			<image src="../../static/logo.png" mode=""></image>
		</view>
		<view class="bar myPhoto">
			<text>
				我的照片
			</text>
			<view class="photo">
				<image src="../../static/logo.png" mode=""></image>
				<image src="../../static/logo.png" mode=""></image>
				<image src="../../static/logo.png" mode=""></image>
			</view>
		</view>
		<view class="bar foot">
			<text>
				姓名:
			</text>
		</view>
		<view class="bar foot">
			<text>
				职业:
			</text>
		</view>
		<view class="bar foot">
			<text>
				婚姻状况:
			</text>
		</view>
			<button @click="gotoAlter()">修改信息</button>
	</view>

</template>

<script>
	export default {
		data() {
			return {
				
			}
		},
		methods: {
			gotoAlter(){
				uni.navigateTo({
					url:"../alter/alter"
				})
			}
		}
	}
</script>

<style>
	.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
		}
	.content .bar{
			width: 600rpx;
			background: #FFE332;
			box-shadow: 0px 4px 10px 1px rgba(0, 0, 0, 0.3019607961177826);
			border-radius: 10px 10px 10px 10px;
			opacity: 1;
			border: 3px solid #733000;
			margin-bottom: 40rpx;
			font-size: 36rpx;
			font-weight: 500;
		}
	.content .head{
			height: 200rpx;
			line-height: 200rpx;
			margin-top: 73rpx;
		}
	.content text{
			margin-left: 48rpx;
		}
	.content .head text{
			margin-top: 42rpx;
		}
	.content .head image{
		float: right;
		width: 160rpx;
		height: 160rpx;
		border-radius: 50%;
		margin: 20rpx 40rpx;
		}
	.content .myPhoto{
		height: 340rpx;
		padding-top: 42rpx;
	}

	.content .myPhoto image{
		width: 160rpx;
		height: 160rpx;
		margin: 0 5rpx;
		margin-top:45rpx ;
		}
	.content .photo {
		margin-left: 40rpx;
	}
	.content .foot{
		height: 100rpx;
		padding-top: 24rpx;
	}
	button{
		width: 100%;
		height: 118rpx;
		line-height: 118rpx;
		background: #733000;
		border: 0px solid #733000;
		color: #fff;
		margin-top: 108rpx;
		border-radius: 0;
	}
</style>
