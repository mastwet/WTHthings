<template>
	<view>
		<view class="item" v-for="(item,index) in personList" :key="index">
			<image src="../../static/logo.png" mode=""></image>
			<view class="text">
				<text>
					{{item.name}}
					{{item.job+" "}} 
				</text>
				<text>
					{{"年薪"+item.income+"万"}}
				</text>
			</view>
			<view class="text_2">
				<text>
					{{"性别:"+item.sexual}}
					{{"婚姻历史:"+item.marriage}}
					{{"年龄:"+item.age}}
					{{"身高:"+item.height}}
				</text>
			</view>
			
			<button @click="getInfo()">联系TA</button>
		</view>
	</view>
</template>
<script src="https://unpkg.com/axios/dist/axios.min.js"></script>

<script>
	export default {
		data() {
			return {
				blank:" ",
				personList:[
						{
						src:"../../static/logo.png"	,
						name:"康然",
						sexual:"男",
						car:true,
						house:false,
						job:"健身教练",
						age:"25岁",
						height:180,
						education:"大专学历",
						income:20,
						marriage:"未婚",
						requirement:"有钱多金",							
						}
					]
				}
		},
		methods: {
			getInfo(){
				var that=this
				axios.get("http://192.168.1.122:8000/personaldetails").then(function(res){
					console.log(res)
				},function(err){
					console.log(err)
				} )
			}
		},
		created(){
			console.log(123)
		}
	}
</script>

<style>
	
	.item{
		width: 680rpx;
		height: 1222rpx;
		margin: auto;
		background: #FFE332;
		box-shadow: 0px 4px 10px 1px rgba(0, 0, 0, 0.3019607961177826);
		border-radius: 60rpx;
		opacity: 1;
		border: 3px solid #733000;
		position: relative;
	}
	.item image{
		width: 608rpx;
		height: 434rpx;
		margin: 63rpx 36rpx 32rpx;
		border-radius: 10rpx;
	}
	.item .text{
		width: 594rpx;
		margin: 0 auto;
		font-size: 24px;
		font-family: Source Han Sans CN-Regular, Source Han Sans CN;
		font-weight: 400;
		color: #733000;
		line-height: 34px;

	}
	.item .text_2{
		width: 608rpx;
		height: 406rpx;
		margin: 18rpx;
		padding-left: 32rpx;
		padding-top: 36rpx;
		font-size: 15px;
		font-family: Source Han Sans CN-Regular, Source Han Sans CN;
		font-weight: 400;
		color: #733000;
		line-height: 21px;
		border: 1px dashed grey;
	}
	button{
		position: absolute;
		bottom: 0rpx;
		width: 100%;
		height: 118rpx;
		line-height: 118rpx;
		background: #733000;
		border: 0px solid #733000;
		color: #fff;
		border-radius: 0rpx 0rpx 50rpx 50rpx;
	}
</style>
