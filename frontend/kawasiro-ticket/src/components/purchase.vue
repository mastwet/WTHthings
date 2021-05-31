<template>
	<v-container>
		<v-card class="mx-auto" max-width="344" outlined>
			<v-list-item three-line>
				<div class="headline">确认订单</div>
			</v-list-item>
			<v-list-item two-line>
				<v-list-item-content outlined>
					<v-list-item-title class="headline mb-1">幻想朱槿绘萃</v-list-item-title>
					<v-list-item-subtitle class="mt-2">由河童软科提供</v-list-item-subtitle>

					<v-list-item-text class="mt-2">
						<p>时间：10月1日<br>地址：南宁市会展中心1区xxx</p>
					</v-list-item-text>


				</v-list-item-content>

				<v-list-item-avatar tile height="200" width="150" color="grey">
					<v-row justify="space-between">
						<v-col cols="auto">
							<v-img height="200" width="200" src="https://cdn.vuetifyjs.com/images/cards/store.jpg"></v-img>
						</v-col>
					</v-row>
				</v-list-item-avatar>
			</v-list-item>

			<v-row>
				<v-col cols="12" md="8">
					<v-divider></v-divider>
				</v-col>
			</v-row>
			<v-col class="d-flex">
				<strong>单格</strong>
				<v-spacer></v-spacer>
				<strong>￥14</strong>
			</v-col>
			<v-col class="d-flex align-sm-center">
				<strong>数量</strong>
				<v-spacer></v-spacer>
				<strong>1</strong>
<!-- 				<v-cols col="8">
					<v-text-field max="50" min="1" step="1" style="width: 40px" type="number"></v-text-field>
				</v-cols> -->
			</v-col>
			<v-col class="d-flex">
				<strong>总价格</strong>
				<v-spacer></v-spacer>
				<strong>￥14</strong>
			</v-col>
			<v-row>
				<v-col cols="12" md="8">
					<v-divider></v-divider>
				</v-col>
			</v-row>
			<v-col class="d-flex align-sm-center" dense>
				<h4>支付方式：</h4>
				<v-radio-group v-model="row" row>
					<v-radio label="微信支付" size="10" value="radio-1"></v-radio>
					<v-radio label="支付宝" value="radio-2"></v-radio>
				</v-radio-group>
			</v-col>
			<v-footer padless light>
				<v-col class="text-center" cols="12">
					去支付
				</v-col>
			</v-footer>
		</v-card>
	</v-container>
</template>

<script>
	export default {
		name: 'purchase',

		data: () => ({
			isEditing: null,
		}),

		methods: {
			save() {
				this.isEditing = !this.isEditing
				this.hasSaved = true
			},
		},
	}
</script>

<style>

</style>
