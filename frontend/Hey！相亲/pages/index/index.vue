<template>
	<view class="content">
							
		<view class="item" v-for="(item,index) in personList" :key="index" v-show="flag">
			<image :src="item.src" mode=""></image>	
			<view class="text">
				<text class="name">
					{{item.name}}
				</text>
					<image src="../../static/images/woman.png" mode="" class="icon" v-if="item.sexual=='女'"></image>
					<image src="../../static/images/car.png" mode="" class="icon" v-if="item.car"></image>
				<text>
					{{blank}}
					{{item.job+"·"}}
				</text>	
				<text>
					{{item.age+"·"}}					
				</text>
				<text>
					{{item.height+"·"}}
					{{item.education+"·"}}					
				</text>
				<text >
					{{item.income+"·"}}
					{{"婚姻状态:"+ item.marriage}}
					{{"择偶要求:"+item.requirement}}
				</text>
			</view>
		</view>
		
		<view class="itemChanged item" v-for="(item,index) in personList" :key="index" v-show="!flag" @click="gotoInformation()">
			<image :src="item.src" mode=""></image>	
			<view class="textChanged">
				<text class="name">
					{{item.name}}
				</text>
					<image src="../../static/images/woman.png" mode="" class="icon" v-if="item.sexual=='女'"></image>
					<image src="../../static/images/car.png" mode="" class="icon" v-if="item.car"></image>
				<text>
					{{blank}}
					{{item.job+"·"}}
				</text>	
				<text>
					{{item.age+"·"}}					
				</text>
				<text>
					{{"身高"+item.height+"·"}}
					{{item.education+"·"}}					
				</text>
				<text >
					{{"年薪"+item.income+"万·"}}
					{{"婚姻状态:"+ item.marriage}}
					{{"择偶要求:"+item.requirement}}
				</text>
				<image src="../../static/images/组@4.png" mode="" class="heart"></image>
				<image src="../../static/images/组@1x(1).png" mode="" class="circle"></image>
			</view>
		</view>
		<button @click="changeView()">切换视角</button>
	</view>
</template>

<script>
	export default {
		data() {
			return {
				blank:" ",
				flag:false,
				personList:[
					{
					src:"../../static/logo.png"	,
					name:"林芊芊",
					sexual:"女",
					car:true,
					house:false,
					job:"职业塔罗师",
					age:"20岁",
					height:165,
					education:"大专学历",
					income:20,
					marriage:"离异",
					requirement:"有钱多金",							
					}
				]
			}
		},
		onLoad() {
		
		},
		methods: {
			changeView(){
				this.flag=!this.flag
			},
			gotoInformation(){
				uni.navigateTo({
					url:"../information/information"
				})
			}
		}
	}
</script>

<style>
		*{
		box-sizing: border-box;
		}
		.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
		}
		button{
			/* width: 100%; */
			height: 118rpx;
			line-height: 118rpx;
			background: #733000;
			border: 0px solid #733000;
			color: #fff;
			margin-top: 108rpx;
			border-radius: 0;
		}
		.item{
			width: 600rpx;
			height: 240rpx;
			background: #FFE332;
			box-shadow: 0px 4px 10px 1px rgba(0, 0, 0, 0.3019607961177826);
			border-radius: 10px 10px 10px 10px;
			opacity: 1;
			border: 3px solid #733000;
			font-weight: 500;
			
		}
		.item image{
			width: 160rpx;
			height: 160rpx;
			margin: 40rpx;
			margin-bottom: 0;
		}
		.item .text{
			font-family:'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;
			float: right;
			margin-right: 100rpx;
			margin-top: 30rpx;
		}
		.item .text .name{
			font-size: 36rpx;
		}
		.item .text text{
			font-size: 22rpx;
		}
		.item .icon{
			width: 40rpx;
			height: 40rpx;
			margin: 0;
		}
		
		/* 另一个视角的css */
		.itemChanged{
			position: relative;
			width: 600rpx;
			height: 728rpx;
		}
		.itemChanged image{
			width: 508rpx;
			height: 400rpx;
		}
		.itemChanged .textChanged{
			width: 508rpx;
			margin: auto;
			/* letter-spacing: 5rpx; */
		}
		.itemChanged .textChanged .name{
			font-size: 50rpx;
		}
		.itemChanged .textChanged text{
			font-size: 36rpx;
		}
		.itemChanged .icon{
			width: 60rpx;
			height: 60rpx;
			margin: 0;
		}
		.itemChanged .heart{
			position: absolute;
			width: 66rpx;
			height: 66rpx;
			right: 10rpx;
			bottom: 7rpx;
		}
		.itemChanged .circle{
			position: absolute;
			width: 86rpx;
			height: 86rpx;
			right: 0rpx;
			bottom: 0rpx;
		}
</style>
