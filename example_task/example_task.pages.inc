<?php

/**
 * Task view callback.
 */
function example_task_view($task) {
  drupal_set_title(entity_label('example_task', $task));
  return entity_view('example_task', array(entity_id('example_task', $task) => $task), 'full');
}
