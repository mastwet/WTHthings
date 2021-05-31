<template>
	<div id="app">
		<v-app id="inspire">
			<v-container fluid>
				<v-col dense>
					<v-row dense>
						<v-card class="mr-auto" max-width="300">
							<v-img class="white--text align-end" height="400px" src="https://wx2.sbimg.cn/2020/08/28/6gNZD.jpg">
								<v-card-title class="purple">幻想朱槿绘翠</v-card-title>
							</v-img>

							<v-card-subtitle class="pb-0">2020年10月2日</v-card-subtitle>

							<v-card-text class="text--primary">
								<div>「暾将出兮东方，照吾槛兮朱槿」</div>

								<div>春去灯盏黯失色，秋来朱槿幸初成——郑重地宣布——东方project主题交流会「幻想朱槿绘翠2」将于2020年10月2日于广西首府南宁市举办！</div>
							</v-card-text>

							<v-card-actions>
								<v-btn color="orange" text>
									查看详情
								</v-btn>

								<v-btn color="orange" text>
									立即购票
								</v-btn>
							</v-card-actions>
						</v-card>
					</v-row>
				</v-col>
				<div fluid>
					<v-btn color="error" @click="overlay = !overlay">
						登录
					</v-btn>

					<v-overlay :value="overlay">
						<v-container>
							<login />
						</v-container>
						<v-btn icon @click="overlay = false">
							<v-icon>mdi-close</v-icon>
						</v-btn>
					</v-overlay>
				</div>
			</v-container>
		</v-app>
	</div>
</template>

<script>
	import login from './login.vue'
	// import HelloWorld from './HelloWorld.vue'
	export default {
		name: 'app_card',
		components: {
			// HelloWorld,
			login,
			// login,
		},
		data: () => ({
			overlay: false,
		})
	}
</script>
<style>
</style>
