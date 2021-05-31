import qrcode
import base64

def make_code(number,cdk):

    str_cdk = base64.b64encode((number+cdk).encode())
    qr = qrcode.QRCode(
        version=None,
        error_correction=qrcode.constants.ERROR_CORRECT_L,
        box_size=10,
        border=2,
    )

    qr.add_data(str_cdk)
    qr.make(fit=True)
    img = qr.make_image()
    img.save('imgcode/{name}.png'.format(name=cdk))