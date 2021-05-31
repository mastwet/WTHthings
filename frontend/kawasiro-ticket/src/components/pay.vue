<template>
	<v-sheet class="text-center" height="200px">
		<v-progress-linear indeterminate color="red darken-2"></v-progress-linear>
		<v-col class="d-flex justify-center">
			<h2>正在付款</h2>
			<v-progress-circular indeterminate></v-progress-circular>
		</v-col>
		<div class="py-3">正在调起第三方支付平台...请稍后...<br>付款成功后您会收到短信，并可以在“我买到的”查看取票码！</div>
		<v-btn text color="red" @click="sheet = !sheet">停止支付</v-btn>
	</v-sheet>
</template>

<script>
	export default {
		name: 'pay',

		data: () => ({
			isEditing: null,
		}),

		methods: {

		},
	}
</script>

<style>
</style>
