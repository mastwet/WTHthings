<template>
	<view class="content">
		<view class="myInfo bar" v-for="(item,index) in person" :key="index">
			<image :src="item.src" mode=""> </image>
			<view class="text">
				<text class="name">
					{{item.name}}
					{{blank}}
				</text>
				<text>
					{{"我的鲜花 "+item.flower}}
					{{"我的余额 "+item.coin}}
				</text>

			</view>
		</view>
		<button class="bar" @click="gotoDetail()">
			<image src="../../static/images/组@1x(1).png" mode=""></image>
			<text>
				我的信息
			</text>
		</button>
		<button class="bar">
			<image src="../../static/images/组@1x(1).png" mode=""></image>
			<text>
				我要上墙
			</text>
		</button>
		<button class="bar">
			<image src="../../static/images/组@1x(1).png" mode=""></image>
			<text>
				寻找红娘
			</text>
		</button>
	</view>
</template>

<script>
	export default {
		data() {
			return {
				blank:" ",
				person:[
					{
						name:"林芊芊",
						flower:1,
						coin:2,
						src:"../../static/logo.png"
					}
				]
			}
		},
		methods: {
			gotoDetail(){
				uni.navigateTo({
					url:"../detail/detail"
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
		background: #FFE332;
		box-shadow: 0px 4px 10px 1px rgba(0, 0, 0, 0.3019607961177826);
		border-radius: 10px 10px 10px 10px;
		opacity: 1;
		border: 3px solid #733000;
	}	
	.content .myInfo{
		width: 600rpx;
		height: 240rpx;
		margin-bottom: 92rpx;
	}
	.content .myInfo image{
		width: 160rpx;
		height: 160rpx;
		border-radius: 50%;
		margin: 40rpx;
	}
	.content button{
		width: 600rpx;
		height: 100rpx;
		margin-bottom: 18rpx;
	}
	.content .text{
		float: right;
		margin-right: 35%;
		margin-top: 10%;
	}
	.content .text{
		font-size: 28rpx;
		font-weight: 500;
	}
	.content .text .name{
		font-size: 48rpx;
	}
	button{
		position: relative;
	}
	button image{
		position: absolute;
		width: 75rpx;
		height: 75rpx;
		left: 142rpx;
		top: 8rpx;
	}
	button text{
		font-size: 40rpx;
		font-weight: 500;
		margin-left: 70rpx;
		line-height: 95rpx;
	}
</style>
