import decorator
import os
import subprocess as sub
import requests
import urllib

def _decode_parameter(value):
    """Get BDD step parameter, redirecting to env var if start with $."""
    if value.startswith('$'):
        return os.environ.get(value[1:], '')
    else:
        return value


def _get_data_from_context(context):
    """Use context.text as a template and render against any stored state."""
    return context.result.getvalue()

@decorator.decorator
def resolve_table_vars( f, context,*parameters):
    context.resolved_table = []
    if hasattr(context,'table'):
        for row in context.table:
            inrow = []
            for column in row:
                if column.strip()[0] == '$':
                    inrow.append(context.alienvault[column[1:].strip()])
                else:
                    inrow.append (column)
            context.resolved_table.append (inrow)

    f(context,*parameters)

@decorator.decorator
def dereference_step_parameters_and_data(f, context, *parameters):
    """Decorator to dereference step parameters and data.

    This involves two parts:

        1) Replacing step parameters with environment variable values if they
        look like an environment variable (start with a "$").

        2) Treating context.text as a Jinja2 template rendered against
        context.template_data, and putting the result in context.data.

    """
    decoded_parameters = map(_decode_parameter, parameters)
    context.data = _get_data_from_context(context)
    f(context, *decoded_parameters)


def make_request(context, url , request_type='GET', is_login=False):
    context.result.truncate(0)
    urlparams = urllib.urlencode(context.urlparams)
    if urlparams != '' and request_type != 'POST':
        url =  url + "?" + urlparams
    elif request_type == 'POST':
        payload = dict(map(lambda x: x.split('='), urlparams.split('&')))

    if hasattr(context,'request_cookies'):
        cookies = dict(cookies_are=";".join(context.request_cookies))
    else:
        cookies = {}

    if request_type == 'GET':
        r = requests.get(url.encode('ascii'), verify=False)
    elif request_type == 'POST':
        r = requests.post(url.encode('ascii'), verify=False, data=payload)
    elif request_type == 'PUT':
        r = requests.put(url.encode('ascii'), verify=False)
    elif request_type == 'DELETE':
        r = requests.delete(url.encode('ascii'), verify=False)
    else:
        return None

    context.result = str(r.text)
    context.resultheader = str(r.headers)
    context.resultcode = str(r.status_code)
    context.resultcookies = str(requests.utils.dict_from_cookiejar(r.cookies))
    if is_login:
        #Save the login cookie to use it in subsequent calls
        context.request_cookies = str(requests.utils.dict_from_cookiejar(r.cookies))

    context.urlparams = {}
    return r


