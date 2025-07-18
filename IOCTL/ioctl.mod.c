#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa8b1ed91, "cdev_init" },
	{ 0x6c34b9d1, "cdev_add" },
	{ 0x92997ed8, "_printk" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xa19b956, "__stack_chk_fail" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbc633891, "cdev_del" },
	{ 0x37a0cba, "kfree" },
	{ 0xf02dd3c3, "kmalloc_caches" },
	{ 0x58ffa277, "kmalloc_trace" },
	{ 0xfe8c61f0, "_raw_read_lock" },
	{ 0xdd4d55b6, "_raw_read_unlock" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x8d6aff89, "__put_user_nocheck_4" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xe68efe41, "_raw_write_lock" },
	{ 0x40235c98, "_raw_write_unlock" },
	{ 0x5219c37a, "module_layout" },
};

MODULE_INFO(depends, "");

