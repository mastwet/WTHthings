<template>
	<v-container>
		<div class="d-flex justify-start">
			<v-breadcrumbs :items="items"></v-breadcrumbs>
		</div>
		<div>
			<v-row class="d-flex justify-start">
				<v-col cols="auto">
					<v-img class="align-end" height="500px" width="300px" src="https://wx2.sbimg.cn/2020/08/28/6gNZD.jpg">
					</v-img>
				</v-col>
				<v-row>
					<v-col>
						<div class="pa-3">
							<h2>东方朱槿绘萃</h2>
						</div>
						<div class="pa-2 mt-2">
							<h4>时间：2020年12月12日</h4>
						</div>
						<div class="pa-2">
							<h4>场馆：南宁市会展中心</h4>
						</div>
						<div class="pa-2">
							<h4>场次：</h4>
						</div>
					</v-col>

				</v-row>
			</v-row>
		</div>
	</v-container>
</template>

<script>
	export default {
		name: 'detail_info',

		data: () => ({
			items: [{
					text: '主页',
					disabled: false,
					href: 'breadcrumbs_dashboard',
				},
				{
					text: '购票详情',
					disabled: false,
					href: 'breadcrumbs_link_1',
				},
				{
					text: '东方朱槿绘萃',
					disabled: true,
					href: 'breadcrumbs_link_2',
				},
			],
		}),

		methods: {

		},
	}
</script>

<style>
</style>
