<?php
/**
 * @file
 * Rules module integration.
 */

/**
 * Implements hook_rules_action_info().
 */
function rules_http_client_rules_action_info() {
  $items['request_url'] = array(
    'label' => t('Request HTTP data'),
    'base' => 'rules_http_client_request_url',
    'group' => t('Data'),
    'parameter' => array(
      'url' => array(
        'type' => 'uri',
        'label' => 'URL',
      ),
      'headers' => array(
        'type' => 'text',
        'label' => 'Headers',
        'description' => 'Request headers to send as "name: value" pairs, one per line (e.g., Accept: text/plain). See <a href="https://www.wikipedia.org/wiki/List_of_HTTP_header_fields">wikipedia.org/wiki/List_of_HTTP_header_fields</a> for more information.',
        'optional' => TRUE,
      ),
      'method' => array(
        'type' => 'text',
        'label' => 'Method',
        'description' => 'The HTTP request method, GET or POST.',
        'options list' => 'rules_http_client_options_method',
        'optional' => TRUE,
      ),
      'data' => array(
        'type' => 'text',
        'label' => 'Data',
        'description' => "The request body. Define this in whichever format fits your Content-Type or not at all depending on your selected HTTP Method.",
        'optional' => TRUE,
      ),
      'max_redirects' => array(
        'type' => 'integer',
        'label' => 'Max redirects',
        'description' => 'How many times a redirect may be followed.',
        'default value' => 3,
        'optional' => TRUE,
      ),
      'timeout' => array(
        'type' => 'decimal',
        'label' => 'Timeout',
        'description' => 'The maximum number of seconds the request may take.',
        'default value' => 30,
        'optional' => TRUE,
      ),
    ),
    'provides' => array(
      'http_response' => array(
        'type' => 'text',
        'label' => t('HTTP data'),
      ),
    ),
  );

  return $items;
}

/**
 * Options list callback for 'Method' parameter.
 */
function rules_http_client_options_method() {
  return array(
    'GET' => 'GET',
    'HEAD' => 'HEAD',
    'POST' => 'POST',
    'PUT' => 'PUT',
    'DELETE' => 'DELETE',
    'TRACE' => 'TRACE',
    'OPTIONS' => 'OPTIONS',
    'CONNECT' => 'CONNECT',
    'PATCH' => 'PATCH',
  );
}

/**
 * Callback for 'request_url' rules action.
 */
function rules_http_client_request_url($url, $headers = '', $method = 'GET', $data = NULL, $max_redirects = 3, $timeout = 30) {
  // Headers.
  if( strpos(':', $headers) >= 0 ){
  	$headers = explode("\r\n", $headers);
  	foreach ($headers as $header) {
    	list($name, $value) = explode(':', $header, 2);
    	if (!empty($name)) {
      		$options['headers'][$name] = ltrim($value);
    	}
  	}
  }

  // Method.
  $method = drupal_strtoupper($method);
  $options['method'] = $method;

  // Data.
  if( isset($data) ){
  	$options['data'] = $data;
  }
  
  // Max redirects.
  $options['max_redirects'] = empty($max_redirects) ? 3 : $max_redirects;

  // Timeout.
  $options['timeout'] = empty($timeout) ? 30 : $timeout;

  // Allow modules to alter the URL and options.
  drupal_alter('rules_http_client_request', $url, $options);

  $response = drupal_http_request($url, $options);
  if (isset($response->error)) {
    watchdog('Rules HTTP Request', 'An error occurred in Drupal Rules HTTP Request. Error Code: ' . $response->code . ' - ' . $response->error);
    return;
  }
  else {
    return array('http_response' => $response->data);
  }
}
