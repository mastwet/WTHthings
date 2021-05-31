<template>
	<v-app id="inspire">
		<div>
			<v-navigation-drawer v-model="drawer" temporary app>
				<v-list-item two-line :class="miniVariant && 'px-0'">
					<v-list-item-avatar>
						<img src="https://randomuser.me/api/portraits/men/81.jpg">
					</v-list-item-avatar>

					<v-list-item-content>
						<v-list-item-title>大湿</v-list-item-title>
						<v-list-item-subtitle>已登录</v-list-item-subtitle>
					</v-list-item-content>
				</v-list-item>
				<v-divider></v-divider>

				<v-list dense>
					<v-list-item link>
						<v-list-item-action>
							<v-icon>mdi-home</v-icon>
						</v-list-item-action>
						<v-list-item-content>
							<v-list-item-title>首页</v-list-item-title>
						</v-list-item-content>
					</v-list-item>
					<v-list-item link>
						<v-list-item-action>
							<v-icon>mdi-email</v-icon>
						</v-list-item-action>
						<v-list-item-content>
							<v-list-item-title>正在热销</v-list-item-title>
						</v-list-item-content>
					</v-list-item>
					<v-list-item link>
						<v-list-item-action>
							<v-icon>mdi-email</v-icon>
						</v-list-item-action>
						<v-list-item-content>
							<v-list-item-title>所有演出</v-list-item-title>
						</v-list-item-content>
					</v-list-item>
					<v-list-item link>
						<v-list-item-action>
							<v-icon>mdi-email</v-icon>
						</v-list-item-action>
						<v-list-item-content>
							<v-list-item-title>我的演出</v-list-item-title>
						</v-list-item-content>
					</v-list-item>
				</v-list>
			</v-navigation-drawer>
			<v-app-bar app color="deep-purple accent-4" dark>
				<v-app-bar-nav-icon @click.stop="drawer = !drawer"></v-app-bar-nav-icon>

				<v-toolbar-title>卖票机</v-toolbar-title>

				<v-spacer></v-spacer>

				<v-col cols="auto" sm="auto" md="auto">

					<v-menu left bottom>
						<template v-slot:activator="{ on, attrs }">
							<v-btn icon v-bind="attrs" v-on="on">
								<v-icon>mdi-dots-vertical</v-icon>
							</v-btn>
						</template>
						<v-dialog v-model="dialog" scrollable max-width="400">
							<template v-slot:activator="{ on, attrs }">
								<v-list>
									<v-list-item v-bind="attrs" v-on="on">
										<v-list-item-title>个人信息</v-list-item-title>
									</v-list-item>
								</v-list>
							</template>
							<user_message />
							<!-- 							<v-card>
								<v-card-title>test</v-card-title>
								<v-card-actions>
									<v-btn color="green darken-1" text v-on:click="dialog = false">
										close
									</v-btn>
								</v-card-actions>
							</v-card> -->
						</v-dialog>
						<v-list>
							<v-list-item @click="() => {}">
								<v-list-item-title>我买到的</v-list-item-title>
							</v-list-item>

						</v-list>

					</v-menu>
				</v-col>
				<strong>大湿</strong>
				<v-col cols="auto" sm="auto" md="auto">
					<v-avatar size="38">
						<img src="https://cdn.vuetifyjs.com/images/john.jpg" alt="John" @click="() => {}">
					</v-avatar>
				</v-col>

			</v-app-bar>
		</div>

		<v-main>
			<detail_info />
			<!--      <HelloWorld/> -->
			<!-- <app_card /> -->
			<!-- 			<app_card /> -->

<!-- 			<div>
				<v-btn color="error" @click="overlay = !overlay">
					个人信息
				</v-btn>

				<v-overlay :value="overlay">
					<v-container :value="overlay">
						<user_message />
					</v-container>
					<v-btn icon @click="overlay = false">
						<v-icon>mdi-close</v-icon>
					</v-btn>
				</v-overlay>
			</div>
			<div>
				<v-btn color="error" @click="overlay2 = !overlay2">
					购买
				</v-btn>

				<v-overlay :value="overlay2">
					<v-container :value="overlay2">
						<purchase />
					</v-container>
					<v-btn icon @click="overlay2 = false">
						<v-icon>mdi-close</v-icon>
					</v-btn>
				</v-overlay>
			</div>
			<div class="text-center">
				<v-btn color="blue" dark @click="sheet = !sheet">
					支付
				</v-btn>
				<v-bottom-sheet v-model="sheet">
					<pay />
				</v-bottom-sheet>
			</div> -->

		</v-main>
		<!-- 		<purchase_footer /> -->
		<!-- 		<v-footer color="deep-purple accent-4" app>
			<span class="white--text">&copy; {{ new Date().getFullYear() }} 河童软科 design&code by 大湿mastwet</span>
		</v-footer> -->
	</v-app>
</template>

<script>
	//import HelloWorld from './components/HelloWorld';
	// import app_card from './components/app_card'
	// import login from './components/login.vue'
	// import user_message from './components/user_message.vue'
	// import purchase_footer from './components/purchase_footer.vue'
	// import purchase from './components/purchase.vue'
	// import pay from './components/pay.vue'
	import detail_info from './components/detail_info.vue'

	export default {
		name: 'App',

		components: {
			//HelloWorld,
			// app_card,
			// user_message,
			// // purchase_footer,
			// // login,
			// purchase,
			// pay,
			detail_info,
		},

		data: () => ({
			drawer: null,
			dialog: false,
			overlay: false,
			overlay2: false,
			sheet: false,
		}),
		methods: {
			print: function() {
				console.log('111');
			}
		}
	}
</script>
