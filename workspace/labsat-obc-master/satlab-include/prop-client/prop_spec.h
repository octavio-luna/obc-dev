/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * System properties are specified by a subsystem property spec tree (struct
 * prop_subsys_spec) which contains a number of property groups (struct
 * prop_group_spec) which again contains a set of properties (struct
 * prop_spec). Each property has a 16-bit ID, a type and a string name.
 *
 * This file contains functions to iterate over the subsystem property spec and
 * to locate properties by ID or name.
 *
 * Subsystem property specs are distributed with the subsystem client library.
 */

#ifndef _SL_PROP_SPEC_H_
#define _SL_PROP_SPEC_H_

#include <prop-client/prop_spec_types.h>

/**
 * @brief Get property ID from spec struct
 *
 * @param spec Pointer to property spec struct.
 *
 * @returns Property ID of property, 0 if NULL is passed.
 */
prop_id_t sl_prop_remote_id(const struct prop_spec *spec);

/**
 * @brief Get property type from spec struct
 *
 * @param spec Pointer to property spec struct.
 *
 * @returns Property type of property, 0 if NULL is passed.
 */
prop_type_t sl_prop_remote_type(const struct prop_spec *spec);

/**
 * @brief Property group spec iterator function
 *
 * Functions implementing this prototype can be passed to
 * sl_prop_group_spec_iterate to iterate over a subsys property group spec.
 *
 * @param group Pointer to current property group spec.
 * @param arg Pointer value passed to sl_prop_group_spec_iterate arg argument.
 *
 * @returns 0 to continue iteration, non-zero to stop iterating.
 */
typedef int (*sl_prop_group_spec_iter_t)(const struct prop_group_spec *group, void *arg);

/**
 * @brief Property spec iterator function
 *
 * Functions implementing this prototype can be passed to sl_prop_spec_iterate
 * to iterate over properties in a property group spec.
 *
 * @param prop Pointer to current property spec.
 * @param arg Pointer value passed to sl_prop_spec_iterate arg argument.
 *
 * @returns 0 to continue iteration, non-zero to stop iterating.
 */
typedef int (*sl_prop_spec_iter_t)(const struct prop_spec *prop, void *arg);

/**
 * @brief Get string representation of property spec type
 *
 * @param prop Pointer to property spec struct.
 *
 * @returns String representation of property type or "?" for unknown values.
 */
const char *sl_prop_spec_type_string(const struct prop_spec *prop);

/**
 * @brief Find property group spec from name
 *
 * Locate pointer to property group spec from name.
 *
 * @param spec Pointer to subsystem property spec.
 * @param name Name of property group to find.
 *
 * @returns Pointer to group spec or NULL if spec could not be found.
 */
const struct prop_group_spec *sl_prop_group_spec_find_by_name(const struct prop_subsys_spec *spec, const char *name);

/**
 * @brief Find property spec from name
 *
 * Locate pointer to property spec from name.
 *
 * @param spec Pointer to subsystem property spec.
 * @param name Name of property to find.
 *
 * @returns Pointer to property spec or NULL if spec could not be found.
 */
const struct prop_spec *sl_prop_spec_find_by_name(const struct prop_subsys_spec *spec, const char *name);

/**
 * @brief Iterate over property groups in a subsystem spec.
 *
 * Call iter function on each property group in subsystem spec. The parameter
 * arg is passed unmodified to the iter function, and can be used to pass
 * context.
 *
 * @param spec Pointer to subsystem property spec.
 * @param iter Pointer to iterate function.
 * @param arg Context argument to pass to iterator function.
 *
 * @returns 0 if iteration was successfully completed, error code otherwise.
 */
int sl_prop_group_spec_iterate(const struct prop_subsys_spec *spec, sl_prop_group_spec_iter_t iter, void *arg);

/**
 * @brief Iterate over properties in a property group spec.
 *
 * Call iter function on each property in group spec. The parameter arg is
 * passed unmodified to the iter function, and can be used to pass context.
 *
 * @param group Pointer to property group spec.
 * @param iter Pointer to iterate function.
 * @param arg Context argument to pass to iterator function.
 *
 * @returns 0 if iteration was successfully completed, error code otherwise.
 */
int sl_prop_spec_iterate(const struct prop_group_spec *group, sl_prop_spec_iter_t iter, void *arg);

#endif /* _SL_PROP_SPEC_H_ */
