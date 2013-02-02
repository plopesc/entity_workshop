<?php

/**
 * Generates the task type editing form.
 */
function example_task_type_form($form, &$form_state, $task_type, $op = 'edit') {

  if ($op == 'clone') {
    $task_type->label .= ' (cloned)';
    $task_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $task_type->label,
    '#description' => t('The human-readable name of this task type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($task_type->type) ? $task_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $task_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'example_task_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this task type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($task_type->description) ? $task_type->description : '',
    '#description' => t('Description about the task type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save task type'),
    '#weight' => 40,
  );

  if (!$task_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete task type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('example_task_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing task_type.
 */
function example_task_type_form_submit(&$form, &$form_state) {
  $task_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  example_task_type_save($task_type);

  // Redirect user back to list of task types.
  $form_state['redirect'] = 'admin/structure/task-types';
}

function example_task_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/task-types/' . $form_state['example_task_type']->type . '/delete';
}

/**
 * Task type delete form.
 */
function example_task_type_form_delete_confirm($form, &$form_state, $task_type) {
  $form_state['task_type'] = $task_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['task_type_id'] = array('#type' => 'value', '#value' => entity_id('example_task_type' ,$task_type));
  return confirm_form($form,
    t('Are you sure you want to delete task type %title?', array('%title' => entity_label('task_type', $task_type))),
    'task/' . entity_id('example_task_type' ,$task_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Task type delete form submit handler.
 */
function example_task_type_form_delete_confirm_submit($form, &$form_state) {
  $task_type = $form_state['task_type'];
  example_task_type_delete($task_type);

  watchdog('example_task_type', '@type: deleted %title.', array('@type' => $task_type->type, '%title' => $task_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $task_type->type, '%title' => $task_type->label)));

  $form_state['redirect'] = 'admin/structure/task-types';
}

/**
 * Page to select task Type to add new task.
 */
function example_task_admin_add_page() {
  $items = array();
  foreach (example_task_types() as $task_type_key => $task_type) {
    $items[] = l(entity_label('example_task_type', $task_type), 'task/add/' . $task_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of task to create.')));
}

/**
 * Add new task page callback.
 */
function example_task_add($type) {
  $task_type = example_task_types($type);

  $task = entity_create('example_task', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('task_type', $task_type))));

  $output = drupal_get_form('example_task_form', $task);

  return $output;
}

/**
 * Task Form.
 */
function example_task_form($form, &$form_state, $task) {
  $form_state['task'] = $task;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Name'),
    '#default_value' => $task->title,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $task->description,
  );

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $task->uid,
  );

  field_attach_form('example_task', $task, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save task'),
    '#submit' => $submit + array('example_task_form_submit'),
  );

  // Show Delete button if we edit task.
  $task_id = entity_id('example_task' ,$task);
  if (!empty($task_id) && example_task_access('edit', $task)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('example_task_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'example_task_form_validate';

  return $form;
}

function example_task_form_validate($form, &$form_state) {

}

/**
 * Task submit handler.
 */
function example_task_form_submit($form, &$form_state) {
  $task = $form_state['task'];

  entity_form_submit_build_entity('example_task', $task, $form, $form_state);

  example_task_save($task);

  $task_uri = entity_uri('example_task', $task);

  $form_state['redirect'] = $task_uri['path'];

  drupal_set_message(t('Task %title saved.', array('%title' => entity_label('example_task', $task))));
}

function example_task_form_submit_delete($form, &$form_state) {
  $task = $form_state['task'];
  $task_uri = entity_uri('example_task', $task);
  $form_state['redirect'] = $task_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function example_task_delete_form($form, &$form_state, $task) {
  $form_state['task'] = $task;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['task_type_id'] = array('#type' => 'value', '#value' => entity_id('example_task' ,$task));
  $task_uri = entity_uri('example_task', $task);
  return confirm_form($form,
    t('Are you sure you want to delete task %title?', array('%title' => entity_label('example_task', $task))),
    $task_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function example_task_delete_form_submit($form, &$form_state) {
  $task = $form_state['task'];
  example_task_delete($task);

  drupal_set_message(t('Task %title deleted.', array('%title' => entity_label('example_task', $task))));

  $form_state['redirect'] = '<front>';
}
