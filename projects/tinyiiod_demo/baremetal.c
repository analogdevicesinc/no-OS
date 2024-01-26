#include <errno.h>
#include <iio/iio-backend.h>
#include <iio/iiod-client.h>
#include <iio-private.h>
#include "baremetal.h"

extern const struct iio_backend iio_external_backend;

struct iio_context_pdata {
	struct iiod_client_pdata io_ctx;
	struct iiod_client *iiod_client;
};

const char *fakexml = "<?xml version=\"1.0\" encoding=\"utf-8\"?> <!DOCTYPE context [ <!ELEMENT context (device | context-attribute)*> <!ELEMENT context-attribute EMPTY> <!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*> <!ELEMENT channel (scan-element?, attribute*)> <!ELEMENT attribute EMPTY> <!ELEMENT scan-element EMPTY> <!ELEMENT debug-attribute EMPTY> <!ELEMENT buffer-attribute EMPTY> <!ATTLIST context name CDATA #REQUIRED version-major CDATA #REQUIRED version-minor CDATA #REQUIRED version-git CDATA #REQUIRED description CDATA #IMPLIED> <!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED> <!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED label CDATA #IMPLIED> <!ATTLIST channel id CDATA #REQUIRED type (input|output) #REQUIRED name CDATA #IMPLIED> <!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED> <!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED> <!ATTLIST debug-attribute name CDATA #REQUIRED> <!ATTLIST buffer-attribute name CDATA #REQUIRED>]> <context name=\"xml\" version-major=\"0\" version-minor=\"25\" version-git=\"v0.25\" description=\"no-OS/projects/wethlink tags/wethlink-v1.0.0-1c02f94f1\" >  <context-attribute name=\"hw_model\" value=\"admv9625\" />  <context-attribute name=\"hw_version\" value=\"-\" />  <context-attribute name=\"hw_serial\" value=\"-\" />  <context-attribute name=\"carrier_model\" value=\"-\" />  <context-attribute name=\"carrier_version\" value=\"-\" />  <context-attribute name=\"carrier_serial\" value=\"-\" />  <device id=\"iio:device0\" name=\"hmc6300\" >   <channel id=\"temp\" type=\"input\" >    <attribute name=\"raw\" filename=\"in_temp_raw\" />   </channel>   <attribute name=\"enabled\" />   <attribute name=\"vco\" />   <attribute name=\"vco_available\" />   <attribute name=\"vco_band\" />   <attribute name=\"vco_lock\" />   <attribute name=\"if_attn\" />   <attribute name=\"temp_en\" />   <attribute name=\"rf_attn\" />   <debug-attribute name=\"direct_reg_access\" />  </device> </context>";

static struct iio_context *
baremetal_create_context(const struct iio_context_params *params, const char *args)
{
	int ret;
	struct iio_context *ctx;
	struct iiod_client *iiod_client;
	const char *ctx_attrs[] = { "ip,ip-addr", "uri" };
	const char *ctx_values[2];
	char uri[255 + 3];
	char *description = "no-OS baremetal backend that enables baremetal usage of libiio.";
	struct iio_context_pdata *pdata;
	
	// printf("\t%s()\n",__func__);

	ctx = iio_context_create_from_backend(&iio_external_backend, description);
	ret = iio_err(ctx);
	if (ret)
		return iio_err_cast(ctx);

	ctx->pdata = calloc(1, sizeof(*ctx->pdata));
	if (!ctx->pdata) {
		ret = -ENOMEM;
		goto end;
	}

	ctx->params = *params;
	ctx->xml = fakexml;

	pdata = zalloc(sizeof(*pdata));
	if (!pdata) {
		ret = -ENOMEM;
		goto end;
	}

	//pdata->io_ctx.fd = fd;
	pdata->io_ctx.params = params;
	pdata->io_ctx.ctx_pdata = pdata;

	// ctx_values[0] = description;
	// ctx_values[1] = "11.22.33.44";

	// prm_dbg(params, "Creating context...\n");
	// ctx = iiod_client_create_context(pdata->iiod_client,
	// 				 &iio_external_backend, description,
	// 				 ctx_attrs, ctx_values,
	// 				 ARRAY_SIZE(ctx_values));
	// ret = iio_err(ctx);
	// if (ret)
	// 	goto end;
	
	iio_context_set_pdata(ctx, pdata);

	return ctx;

end:
	iio_context_destroy(ctx);
	return iio_ptr(ret);
}

static ssize_t
baremetal_read_attr(const struct iio_attr *attr, char *dst, size_t len)
{
	// printf("\t%s()\n",__func__);
	return 0;
}

static ssize_t
baremetal_write_attr(const struct iio_attr *attr, const char *src, size_t len)
{
	// printf("\t%s()\n",__func__);
	return 0;
}

const struct iio_backend_ops ops = {
	.create = baremetal_create_context,
	.read_attr = baremetal_read_attr,
	.write_attr = baremetal_write_attr,
};

const struct iio_backend iio_external_backend = {
	.name = "baremetal",
	.api_version = IIO_BACKEND_API_V1,
	.default_timeout_ms = 200,
	.uri_prefix = "baremetal:",
	.ops = &ops,
};